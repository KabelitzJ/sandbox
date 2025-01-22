#ifndef DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
#define DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_

#include <imgui.h>

#include <demo/imgui/bindings/imgui.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/scenes/scenes_module.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <demo/imgui/pipeline.hpp>

namespace demo {

class imgui_subrenderer final : public sbx::graphics::subrenderer {

  inline static constexpr auto ini_file = std::string_view{"demo/assets/data/imgui.ini"};

  using base = sbx::graphics::subrenderer;

public:

  imgui_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : base{stage},
    _pipeline{path, stage},
    _show_demo_window{false},
    _clear_color{sbx::math::color::black},
    _selected_node_id{sbx::math::uuid::null} {
    // Initialize ImGui
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();

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

  ~imgui_subrenderer() override {
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
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

  auto _setup_dockspace() -> void {
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);

    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_MenuBar; // Add this flag to enable the menu bar
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

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
    ImGuiID dockspace_id = ImGui::GetID("DockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    ImGui::End();
  }

  auto _foo() -> void {
    const auto delta_time = sbx::core::engine::delta_time();

    _time += delta_time;
    ++_frames;

    if (_time >= sbx::units::second{1}) {
      _fps = _frames;
      _time = sbx::units::second{0};
      _frames = 0;
    }

    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    ImGui::DockSpaceOverViewport();

    ImGui::Begin("Stats");
    ImGui::Text("Delta time:  %.3f", sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value());
    ImGui::Text("FPS:         %d", _fps);

    auto node = scene.root();

    _build_tree(node);

    ImGui::End();

    _build_node_preview();
  }

  auto _build_tree(sbx::scenes::node& node) -> void {
    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    const auto& relationship = node.get_component<sbx::scenes::relationship>();

    auto flag = ImGuiTreeNodeFlags{ImGuiTreeNodeFlags_OpenOnArrow};
    // auto flag = ImGuiTreeNodeFlags{ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow};

    if (relationship.children().empty()) {
      flag |= ImGuiTreeNodeFlags_Leaf;
    }

    if (node.get_component<sbx::scenes::id>() == _selected_node_id) {
      flag |= ImGuiTreeNodeFlags_Selected;
    }

    if (ImGui::TreeNodeEx(node.get_component<sbx::scenes::tag>().data(), flag)) {
      if (ImGui::IsItemClicked()) {
        _selected_node_id = node.get_component<sbx::scenes::id>();

        sbx::core::logger::debug("Selected node id {}", _selected_node_id);
      }

      for (const auto& child_id : relationship.children()) {
        if (auto child = scene.find_node(child_id); child) {
          _build_tree(*child);
        }
      }

      ImGui::TreePop();
    } 
  }

  auto _build_node_preview() -> void {
    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    ImGui::Begin("Node");

    if (auto node = scene.find_node(_selected_node_id); node) {
      if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
        static auto buffer = std::array<char, 32u>{};
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

        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150); // Adjust slider width
        ImGui::DragFloat("##XPosition", &position.x(), 0.1f);

        ImGui::SameLine();
        ImGui::Text("y:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##YPosition", &position.y(), 0.1f);

        ImGui::SameLine();
        ImGui::Text("z:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##ZPosition", &position.z(), 0.1f);

        ImGui::Text("Scale");

        auto& scale = transform.scale();

        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##XScale", &scale.x(), 0.1f, 0.1f, 10.0f);

        ImGui::SameLine();
        ImGui::Text("y:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##YScale", &scale.y(), 0.1f, 0.1f, 10.0f);

        ImGui::SameLine();
        ImGui::Text("z:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::DragFloat("##ZScale", &scale.z(), 0.1f, 0.1f, 10.0f);

        ImGui::TreePop();
      }
    }

    ImGui::End();
  }

  auto _setup_windows() -> void {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    // Example Window 1
    {
      ImGui::Begin("Hierarchy");

      ImGui::Text("Hello, world!");
      if (ImGui::Button("Demo Window")) {
        sbx::core::logger::info("Button pressed");
      }

      ImGui::End();
    }

    auto vMax = ImVec2{};
    auto vMin = ImVec2{};

    // Example Window 2
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

    // Example Window 3
    {
      ImGui::Begin("Properties");

      auto width = vMax.x - vMin.x;
      auto height = vMax.y - vMin.y;

      ImGui::Text("Width: %f", width);
      ImGui::Text("Height: %f", height);

      ImGui::End();
    }

    // Example Window 4
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

  imgui::pipeline _pipeline;
  sbx::graphics::descriptor_handler _descriptor_handler;

  bool _show_demo_window;
  sbx::math::color _clear_color;

  sbx::units::second _time;
  std::uint32_t _frames;
  std::uint32_t _fps;

  bool _has_auto_scroll;

  sbx::math::uuid _selected_node_id;

};

} // namespace demo

#endif // DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
