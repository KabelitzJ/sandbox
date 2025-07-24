#ifndef LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
#define LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_

#include <vector>
#include <variant>
#include <string>
#include <memory>
#include <unordered_map>
#include <optional>
#include <functional>

#include <vulkan/vulkan.h>

#include <libsbx/math/color.hpp>

#include <libsbx/memory/observer_ptr.hpp>

namespace sbx::graphics {

namespace detail {

// enum class node_state : std::uint32_t {
//   none           = 0x00000000,
//   conditioned    = 0x10000000,
//   preempted      = 0x20000000,
//   retain_subflow = 0x40000000,
//   joined_subflow = 0x80000000
// }; // enum class node_state

// enum class error_state : std::uint32_t {
//   none      = 0x00000000,
//   exception = 0x10000000,
//   cancelled = 0x20000000,
//   anchored  = 0x40000000
// }; // enum class node_state

template<typename T, typename>
struct get_index;

template<std::size_t I, typename... Ts>
struct get_index_impl {};

template<std::size_t I, typename T, typename... Ts>
struct get_index_impl<I, T, T, Ts...> : std::integral_constant<std::size_t, I>{};

template<std::size_t I, typename T, typename U, typename... Ts>
struct get_index_impl<I, T, U, Ts...> : get_index_impl<I+1u, T, Ts...>{};

template<typename T, typename... Ts>
struct get_index<T, std::variant<Ts...>> : get_index_impl<0u, T, Ts...>{};

template<typename T, typename... Ts>
constexpr auto get_index_v = get_index<T, Ts...>::value;

class graph_node;
class graph_base;
class graph_builder;
class task;
class sub_graph;
class graph_context;
class graphics_pass;
class compute_pass;

class graph_base : std::vector<std::unique_ptr<graph_node>> {

  friend class graph_node;
  friend class graph_builder;

  using base = std::vector<std::unique_ptr<graph_node>>;

public:

  graph_base() = default;

  graph_base(const graph_base& other) = delete;

  graph_base(graph_base&& other) = default;

  auto operator=(const graph_base& other) -> graph_base& = delete;

  auto operator=(graph_base&& other) -> graph_base& = default;

private:

  auto _reserve(const std::size_t capacity) -> void;

  auto _erase(graph_node* node) -> void;

  template<typename... Args>
  auto _emplace_back(Args&&... args) -> graph_node*;

}; // class graph_base

struct graphics_pass_parameters {
  std::string name;
  void* data;
}; // struct graphics_pass_parameters

struct default_graphics_pass_parameters { };

template<typename Type>
constexpr bool is_graphics_pass_params_v = std::is_same_v<std::decay_t<Type>, graphics_pass_parameters> || std::is_same_v<std::decay_t<Type>, default_graphics_pass_parameters> || std::is_constructible_v<std::string, Type>;

template<typename Callable, typename = void>
struct is_graphics_pass : std::false_type{ };

template<typename Callable>
struct is_graphics_pass<Callable, std::enable_if_t<std::is_invocable_r_v<graphics_pass&, Callable, graph_context&>>> : std::true_type { };

template <typename Callable>
constexpr bool is_graphics_pass_v = is_graphics_pass<Callable>::value;

struct compute_pass_parameters {
  std::string name;
  void* data;
}; // struct graphics_pass_parameters

struct default_compute_pass_parameters { };

template<typename Type>
constexpr bool is_compute_pass_params_v = std::is_same_v<std::decay_t<Type>, compute_pass_parameters> || std::is_same_v<std::decay_t<Type>, default_compute_pass_parameters> || std::is_constructible_v<std::string, Type>;

template<typename Callable, typename = void>
struct is_compute_pass : std::false_type{ };

template<typename Callable>
struct is_compute_pass<Callable, std::enable_if_t<std::is_invocable_r_v<compute_pass&, Callable, graph_context&>>> : std::true_type { };

template <typename Callable>
constexpr bool is_compute_pass_v = is_compute_pass<Callable>::value;

enum class format : std::uint32_t {
  undefined = VK_FORMAT_UNDEFINED,
  r32_sfloat = VK_FORMAT_R32_SFLOAT,
  r32_uint = VK_FORMAT_R32_UINT,
  r64_uint = VK_FORMAT_R64_UINT,
  r32g32_sfloat = VK_FORMAT_R32G32_SFLOAT,
  r32g32_uint = VK_FORMAT_R32G32_UINT,
  r8g8b8a8_unorm = VK_FORMAT_R8G8B8A8_UNORM,
  b8g8r8a8_srgb = VK_FORMAT_B8G8R8A8_SRGB,
  r32g32b32a32_sfloat = VK_FORMAT_R32G32B32A32_SFLOAT
}; // enum class format

enum class address_mode : std::uint32_t {
  repeat = VK_SAMPLER_ADDRESS_MODE_REPEAT,
  clamp_to_edge = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
}; // enum class address_mode

class attachment {

public:

  enum class type {
    image,
    depth,
    storage,
    swapchain
  }; // enum class type

  attachment(const std::uint32_t binding, const std::string& name, type type, const math::color& clear_color = math::color::black(), const format format = format::r8g8b8a8_unorm, const address_mode address_mode = address_mode::repeat) noexcept
  : _binding{binding}, 
    _name{std::move(name)}, 
    _type{type},
    _clear_color{clear_color},
    _format{format}, 
    _address_mode{address_mode} { }

  auto binding() const noexcept -> std::uint32_t {
    return _binding;
  }

  auto name() const noexcept -> const std::string& {
    return _name;
  }

  auto image_type() const noexcept -> type {
    return _type;
  }

  auto format() const noexcept -> detail::format {
    return _format;
  }

  auto address_mode() const noexcept -> detail::address_mode {
    return _address_mode;
  }

  auto clear_color() const noexcept -> const math::color& {
    return _clear_color;
  }

private:

  std::uint32_t _binding;
  std::string _name;
  type _type;
  bool _is_multi_sampled;
  math::color _clear_color;
  detail::format _format;
  detail::address_mode _address_mode;

}; // class attachment

class graph_node {
  
  friend class graph_builder;
  friend class pass;

  struct graphics_pass_node {

    template<typename Callable>
    graphics_pass_node(Callable&& callable);
    
    std::function<void(graph_context&)> work;
  }; // struct graphics_pass_node

  struct compute_pass_node {

    template<typename Callable>
    compute_pass_node(Callable&& callable);

    std::function<void(graph_context&)> work;
  }; // struct compute_pass_node

  using node_handle = std::variant<std::monostate, graphics_pass_node, compute_pass_node>;

public:

  inline static constexpr auto placeholder = get_index_v<std::monostate, node_handle>;
  inline static constexpr auto graphics_pass = get_index_v<graphics_pass_node, node_handle>;
  inline static constexpr auto compute_pass = get_index_v<compute_pass_node, node_handle>;

  graph_node();
  
  template<typename... Args>
  graph_node(const graphics_pass_parameters& parameters, graph_node* parent, Args&&...);
  
  template<typename... Args>
  graph_node(const default_graphics_pass_parameters& parameters, graph_node* parent, Args&&...);

  auto num_successors() const -> std::size_t;
  auto num_predecessors() const -> std::size_t;

  auto name() const -> const std::string&;

  auto attachments() const -> const std::vector<attachment>&;

private:

  auto _precede(graph_node* node) -> void;
  auto _remove_successors(graph_node* node) -> void;
  auto _remove_predecessors(graph_node* node) -> void;

  // node_state _state;
  std::string _name;
  void* _data;
  graph_node* _parent;
  std::size_t _num_successors;
  std::vector<graph_node*> _edges;
  node_handle _handle;

  std::vector<attachment> _attachments;

}; // class graph_node

class pass {

  friend class graph_builder;

public:

  auto name() const -> const std::string&;

  auto num_predecessors() const -> std::size_t;
  auto num_successors() const -> std::size_t;

  template<typename... Passes>
  auto precede(Passes&&... passes) -> pass&;

  template<typename... Passes>
  auto succeed(Passes&&... passes) -> pass&;

  auto attachments() const -> const std::vector<attachment>& {
    return _node->attachments();
  }

private:

  pass(graph_node* node);

  graph_node* _node;

}; // class pass

class graph_builder {

public:

  graph_builder(graph_base& graph);

  template <typename Callable>
  requires (is_graphics_pass_v<Callable>)
  auto emplace(Callable&& callable) -> pass;

  template <typename Callable>
  requires (is_compute_pass_v<Callable>)
  auto emplace(Callable&& callable) -> pass;

  template<typename... Callables>
  requires (sizeof...(Callables) > 1u)
  auto emplace(Callables&&... callables) -> decltype(auto);

protected:

  graph_base& _graph;

private:

}; // class graph_builder

class graphics_pass {

public:

  auto input(const std::string& name) -> void {
    // _inputs.emplace_back(std::forward<Args>(args)...);
  }

  template<typename... Args>
  requires (std::is_constructible_v<attachment, Args...>)
  auto output(Args&&... args) -> void {
    // _outputs.emplace_back(std::forward<Args>(args)...);
  }

}; // class graphics_pass

class compute_pass {

}; // class compute_pass

class graph_context {

public:

  auto graphics_pass(const std::string& name) -> detail::graphics_pass& {
    return _graphics_pass;
  }

  auto compute_pass(const std::string& name) -> detail::compute_pass& {
    return _compute_pass;
  }

private:

  detail::graphics_pass _graphics_pass;
  detail::compute_pass _compute_pass;

}; // class graph_context

} // namespace detail 

using format = detail::format;
using address_mode = detail::address_mode;

class render_graph : public detail::graph_builder {

  using base = detail::graph_builder; 

public:

  using pass = detail::pass;
  using graphics_pass = detail::graphics_pass;
  using compute_pass = detail::compute_pass;
  using attachment = detail::attachment;
  using context = detail::graph_context;

  render_graph(const std::string& name);

private:

  detail::graph_base _graph;
  std::string _name;

  std::vector<attachment> _attachments;

}; // class render_graph

} // namespace sbx::graphics

#include <libsbx/graphics/render_graph.ipp>

#endif // LIBSBX_GRAPHICS_RENDER_GRAPH_HPP_
