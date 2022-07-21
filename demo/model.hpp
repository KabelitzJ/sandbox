#ifndef DEMO_MODEL_HPP_
#define DEMO_MODEL_HPP_

#include <array>

#include <utils/noncopyable.hpp>

#include "buffer.hpp"
#include "vertex.hpp"
#include "physical_device.hpp"
#include "logical_device.hpp"
#include "command_pool.hpp"

namespace demo {

// [TODO] KAJ 2022-07-21 04:34 - This class is purely temporary till the buffer class is designed properly

template<std::size_t VertexCount, std::size_t IndexCount>
class model : public sbx::noncopyable {

public:

  model(physical_device* physical_device, logical_device* logical_device, command_pool* command_pool, const std::array<vertex, VertexCount>& vertices, const std::array<sbx::uint32, IndexCount>& indices)
  : _vertex_buffer{physical_device, logical_device, command_pool, vertices},
    _index_buffer{physical_device, logical_device, command_pool, indices} { }

  ~model() = default;

  const buffer<vertex, VertexCount, buffer_type::vertex_buffer>& vertex_buffer() const noexcept {
    return _vertex_buffer;
  }

  const buffer<sbx::uint32, IndexCount, buffer_type::index_buffer>& index_buffer() const noexcept {
    return _index_buffer;
  }

private:

  buffer<vertex, VertexCount, buffer_type::vertex_buffer> _vertex_buffer{};
  buffer<sbx::uint32, IndexCount, buffer_type::index_buffer> _index_buffer{};

}; // class model

} // namespace demo

#endif // DEMO_MODEL_HPP_
