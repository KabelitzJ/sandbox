#ifndef LIBSBX_EDITOR_EDITOR_SUBRENDERER_HPP_
#define LIBSBX_EDITOR_EDITOR_SUBRENDERER_HPP_

#include <deque>

#include <imgui.h>
#include <implot.h>

#include <libsbx/editor/bindings/imgui.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>
#include <libsbx/scenes/components/id.hpp>
#include <libsbx/scenes/components/relationship.hpp>
#include <libsbx/scenes/components/tag.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/descriptor/descriptor_handler.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <libsbx/editor/pipeline.hpp>

namespace sbx::editor {

class editor_subrenderer final : public sbx::graphics::subrenderer {

  inline static constexpr auto ini_file = std::string_view{"demo/assets/data/imgui.ini"};

  using base = sbx::graphics::subrenderer;

public:

  editor_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : base{stage},
    _pipeline{path, stage},
    _show_demo_window{false},
    _clear_color{sbx::math::color::black},
    _selected_node_id{sbx::math::uuid::null} {
    // Initialize ImGui
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImPlot::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = ini_file.data();

    ImGui::StyleColorsDark();


    auto& device_module = sbx::core::engine::get_module<sbx::devices::devices_module>();
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    // Connect ImGui to GLFW
    ImGui_ImplGlfw_InitForVulkan(device_module.window(), true);

    // Connect ImGui to Vulkan
    auto init_info = ImGui_ImplVulkan_InitInfo{};
    std::memset(&init_info, 0, sizeof(ImGui_ImplVulkan_InitInfo));
    init_info.Instance = graphics_module.instance();
    init_info.PhysicalDevice = graphics_module.physical_device();
    init_info.Device = graphics_module.logical_device();
    init_info.QueueFamily = graphics_module.logical_device().queue<sbx::graphics::queue::type::graphics>().family();
    init_info.Queue = graphics_module.logical_device().queue<sbx::graphics::queue::type::graphics>();
    init_info.DescriptorPool = _pipeline.descriptor_pool();
    init_info.Subpass = 0;
    init_info.MinImageCount = 2u;
    init_info.ImageCount = graphics_module.swapchain().image_count();
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    ImGui_ImplVulkan_Init(&init_info, graphics_module.render_stage(stage).render_pass());

    // Upload fonts
    {
      auto command_buffer = sbx::graphics::command_buffer{true};

      ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

      command_buffer.submit_idle();

      ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
  }

  ~editor_subrenderer() override {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    _pipeline.bind(command_buffer);

    _descriptor_handler.push("sTexture", graphics_module.attachment("scene"));

    if (!_descriptor_handler.update(_pipeline)) {
      return;
    }

    _descriptor_handler.bind_descriptors(command_buffer);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    _setup_dockspace();
    _setup_windows();

    ImGui::Render();
    auto* draw_data = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
  }

private:

  enum class popup : std::uint16_t {
    hierarchy_add_new_node = sbx::utility::bit_v<0u>,
    hierarchy_add_component = sbx::utility::bit_v<1u>,
    hierarchy_delete = sbx::utility::bit_v<2u>
  }; // enum class popup

  auto _setup_dockspace() -> void {
    const auto* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    const auto window_flags = ImGuiWindowFlags{
      ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_MenuBar |
      ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoBringToFrontOnFocus
    };

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

    ImGui::Begin("DockSpaceWithMenuBar", nullptr, window_flags);
    ImGui::PopStyleVar(2);

    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Save", "Ctrl+S")) { 
          _save(); 
        }
        if (ImGui::MenuItem("Load", "Ctrl+L")) { 
          _load(); 
        }
        if (ImGui::MenuItem("Exit")) {
          sbx::core::engine::quit();
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit")) {
        if (ImGui::MenuItem("Undo", "Ctrl+Z")) { 
          _undo();
        }
        ImGui::EndMenu();
      }
      ImGui::EndMenuBar();
    }

    // Create the dock space
    const auto dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
  }

  auto _context_menu(sbx::scenes::node& node) -> void {
    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    if (ImGui::BeginPopupContextItem("Actions")) {
      if (ImGui::BeginMenu("Add")) {
        if (ImGui::MenuItem("New Node")) {
          _open_popups.set(popup::hierarchy_add_new_node);
        }

        if (ImGui::MenuItem("Component")) {
          _open_popups.set(popup::hierarchy_add_component);
        }

        if (ImGui::MenuItem("Test")) {
          
        }

        ImGui::EndMenu();
      }

      if (_selected_node_id != scene.root().get_component<sbx::scenes::id>()) {
        if (ImGui::MenuItem("Delete")) {
          _open_popups.set(popup::hierarchy_delete);
        }
      }

      ImGui::EndPopup();
    }
  }

  auto _build_tree(sbx::scenes::node& node) -> void {
    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    const auto& relationship = node.get_component<sbx::scenes::relationship>();

    // auto flag = ImGuiTreeNodeFlags{ImGuiTreeNodeFlags_OpenOnArrow};
    auto flag = ImGuiTreeNodeFlags{ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow};

    if (relationship.children().empty()) {
      flag |= ImGuiTreeNodeFlags_Leaf;
    }

    if (node.get_component<sbx::scenes::id>() == _selected_node_id) {
      flag |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushID(&node);

    if (ImGui::TreeNodeEx(node.get_component<sbx::scenes::tag>().c_str(), flag)) {
      if (ImGui::IsItemClicked(ImGuiMouseButton_Right) || ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        _selected_node_id = node.get_component<sbx::scenes::id>();

        sbx::core::logger::debug("Selected node id {}", _selected_node_id);
      }

      _context_menu(node);

      for (const auto& child_id : relationship.children()) {
        if (auto child = scene.find_node(child_id); child) {
          _build_tree(*child);
        }
      }

      ImGui::TreePop();
    }

    ImGui::PopID();
  }

  auto _build_node_preview() -> void {
    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    static auto buffer = std::array<char, 32u>{};

    if (auto node = scene.find_node(_selected_node_id); node) {
      if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
        buffer.fill('\0');

        auto& tag = node->get_component<sbx::scenes::tag>();

        std::memcpy(buffer.data(), tag.data(), std::min(tag.size(), buffer.size()));

        if (ImGui::InputText("##label", buffer.data(), buffer.size())) {
          tag = buffer.data();
        }

        ImGui::TreePop();
      }

      if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto& transform = node->get_component<sbx::math::transform>();

        ImGui::Text("Position");

        auto& position = transform.position();

        ImGui::Text("x");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100); // Adjust slider width
        ImGui::DragFloat("##XPosition", &position.x(), 0.1f);
        ImGui::SameLine();

        ImGui::Text("y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##YPosition", &position.y(), 0.1f);
        ImGui::SameLine();

        ImGui::Text("z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##ZPosition", &position.z(), 0.1f);

        ImGui::Text("Scale");

        auto& scale = transform.scale();

        ImGui::Text("x");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##XScale", &scale.x(), 0.1f, 0.1f, 10.0f);
        ImGui::SameLine();

        ImGui::Text("y");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##YScale", &scale.y(), 0.1f, 0.1f, 10.0f);
        ImGui::SameLine();

        ImGui::Text("z");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(100);
        ImGui::DragFloat("##ZScale", &scale.z(), 0.1f, 0.1f, 10.0f);
        ImGui::SameLine();

        ImGui::TreePop();
      }
    }
  }

  auto _setup_windows() -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    {
      ImGui::Begin("Hierarchy");

      auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

      auto& scene = scene_module.scene();

      auto root = scene.root();

      _build_tree(root);

      const auto custom_backdrop_color = ImVec4{0.2f, 0.2f, 0.2f, 0.5f};

      ImGui::PushStyleColor(ImGuiCol_ModalWindowDimBg, custom_backdrop_color);

      if (_open_popups.has(popup::hierarchy_add_new_node)) {
        _open_popups.clear(popup::hierarchy_add_new_node);
        ImGui::OpenPopup("New Node");
      }

      ImGui::SetNextWindowSizeConstraints(ImVec2{200, 120}, ImVec2{FLT_MAX, FLT_MAX});

      if (ImGui::BeginPopupModal("New Node", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Name");
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
        ImGui::InputText("##NodeNameInput", _new_name_buffer.data(), _new_name_buffer.size());

        const auto name = std::string{_new_name_buffer.data(), std::strlen(_new_name_buffer.data())};

        const auto button_width = 75.0f;
        const auto padding = 10.0f;
        const auto available_width = ImGui::GetContentRegionAvail().x;
        const auto total_width = (button_width * 2.0f) + padding;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available_width - total_width);

        const auto footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - footer_height);

        if (ImGui::Button("Cancel", ImVec2{button_width, 0})) {
          _new_name_buffer.fill('\0');
          ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (name.empty()) {
          ImGui::BeginDisabled();
        }

        if (ImGui::Button("Create", ImVec2{button_width, 0})) {
          if (auto node = scene.find_node(_selected_node_id); node) {
            scene.create_child_node(*node, name);
          } else {
            sbx::core::logger::warn("No selected node");
          }

          _new_name_buffer.fill('\0');
          ImGui::CloseCurrentPopup();
        }

        if (name.empty()) {
          ImGui::EndDisabled();
        }

        ImGui::EndPopup();
      }

      if (_open_popups.has(popup::hierarchy_add_component)) {
        _open_popups.clear(popup::hierarchy_add_component);
        ImGui::OpenPopup("Add Component");
      }

      ImGui::SetNextWindowSizeConstraints(ImVec2{200, 120}, ImVec2{FLT_MAX, FLT_MAX});

      if (ImGui::BeginPopupModal("Add Component", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Not implemented");

        if (ImGui::Button("Close")) {
          ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
      }

      if (_open_popups.has(popup::hierarchy_delete)) {
        _open_popups.clear(popup::hierarchy_delete);
        ImGui::OpenPopup("Delete Node");
      }

      ImGui::SetNextWindowSizeConstraints(ImVec2{200, 120}, ImVec2{FLT_MAX, FLT_MAX});

      if (ImGui::BeginPopupModal("Delete Node", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        const auto button_width = 75.0f;
        const auto padding = 10.0f;
        const auto available_width = ImGui::GetContentRegionAvail().x;
        const auto total_width = (button_width * 2.0f) + padding;

        if (auto node = scene.find_node(_selected_node_id); node) {
          ImGui::Text("Do you want to delete '%s'", node->get_component<sbx::scenes::tag>().c_str());

          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available_width - total_width);

          const auto footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
          ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - footer_height);

          if (ImGui::Button("Cancel", ImVec2{button_width, 0})) {
            ImGui::CloseCurrentPopup();
          }

          ImGui::SameLine();

          if (ImGui::Button("Delete", ImVec2{button_width, 0})) {
            scene.destroy_node(*node);
            _selected_node_id = sbx::math::uuid::null;
            ImGui::CloseCurrentPopup();
          }
        } else {
          ImGui::Text("No node selected (This can be considered a programmer error)");

          if (ImGui::Button("Cancel", ImVec2{button_width, 0})) {
            ImGui::CloseCurrentPopup();
          }
        }

        ImGui::EndPopup();
      }

      ImGui::PopStyleColor();

      ImGui::End();
    }

    {
      ImGui::Begin("Stats");

      auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

      auto& scene = scene_module.scene();

      const auto delta_time = sbx::core::engine::delta_time();

      _time += delta_time;
      ++_frames;

      if (_time >= sbx::units::second{1}) {
        _fps = _frames;
        _time = sbx::units::second{0};
        _frames = 0;
      }

      ImGui::Text("Delta time:  %.3f", sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value());
      ImGui::Text("FPS:         %d", _fps);

      static constexpr auto max_time = sbx::units::second{5.0f};

      _elapsed += delta_time;

      _deltas.push_back(sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value());
      _time_stamps.push_back(_elapsed.value());

      while (!_time_stamps.empty() && (_elapsed - sbx::units::second{_time_stamps.front()} > max_time)) {
        _deltas.erase(_deltas.begin());
        _time_stamps.erase(_time_stamps.begin());
      }

      if (ImPlot::BeginPlot("Delta Time Graph", ImVec2{-1, 150})) {
        ImPlot::SetupAxes("Time (s)", "Delta Time (ms)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
        ImPlot::SetupAxisLimits(ImAxis_X1, (_elapsed - max_time).value(), _elapsed.value(), ImGuiCond_Always);
        ImPlot::PlotShaded("Delta Time", _time_stamps.data(), _deltas.data(), _deltas.size(), 0.0f);
        ImPlot::EndPlot();
      }

      ImGui::End();
    }

    auto vMax = ImVec2{};
    auto vMin = ImVec2{};

    {
      ImGui::Begin("Scene");

      auto current_frame = graphics_module.current_frame();
      auto available_size = ImGui::GetContentRegionAvail();

      ImGui::Image(reinterpret_cast<ImTextureID>(_descriptor_handler.descriptor_set()), available_size);

      if (ImGui::IsItemHovered()) {
        ImGuiIO& io = ImGui::GetIO();
        io.WantCaptureMouse = false; 
        io.WantCaptureKeyboard = false;
      }

      vMin = ImGui::GetWindowContentRegionMin();
      vMax = ImGui::GetWindowContentRegionMax();

      vMin.x += ImGui::GetWindowPos().x;
      vMin.y += ImGui::GetWindowPos().y;
      vMax.x += ImGui::GetWindowPos().x;
      vMax.y += ImGui::GetWindowPos().y;

      ImGui::End();
    }

    {
      ImGui::Begin("Properties");

      auto width = vMax.x - vMin.x;
      auto height = vMax.y - vMin.y;

      ImGui::Text("Width: %f", width);
      ImGui::Text("Height: %f", height);

      _build_node_preview();

      ImGui::End();
    }

    {
      ImGui::Begin("Log");
    
      if (ImGui::Button("Clear"))  {
        sbx::core::logger::info("Clearing log");
      }

      ImGui::SameLine();

      ImGui::Checkbox("Auto-Scroll", &_has_auto_scroll);

      ImGui::Separator();

      ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

      if (_has_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(0.999f);
      }

      ImGui::EndChild();

      ImGui::End();
    }
  }

  auto _save() -> void {
    // [TODO] KAJ 2024-12-02 : Implement save
  }

  auto _load() -> void {
    // [TODO] KAJ 2024-12-02 : Implement load
  }

  auto _undo() -> void {
    // [TODO] KAJ 2024-12-02 : Implement undo
  }

  pipeline _pipeline;
  graphics::descriptor_handler _descriptor_handler;

  bool _show_demo_window;
  math::color _clear_color;

  utility::bit_field<popup> _open_popups;

  sbx::units::second _time;
  std::uint16_t _frames;
  std::uint16_t _fps;

  bool _has_auto_scroll;

  std::array<char, 32> _new_name_buffer;

  math::uuid _selected_node_id;

  std::vector<std::float_t> _deltas;
  std::vector<std::float_t> _time_stamps;
  sbx::units::second _elapsed;

}; // class editor_subrenderer

} // namespace sbx::editor

#endif // LIBSBX_EDITOR_EDITOR_SUBRENDERER_HPP_
