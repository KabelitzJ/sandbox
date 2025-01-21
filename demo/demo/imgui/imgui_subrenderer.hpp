#ifndef DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
#define DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_

#include <imgui.h>

#include <demo/imgui/bindings/imgui_impl_glfw.h>
#include <demo/imgui/bindings/imgui_impl_vulkan.h>

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

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    ImGui::GetIO().IniFilename = nullptr;

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

    ImGui::LoadIniSettingsFromDisk(ini_file.data());
  }

  ~imgui_subrenderer() override {
    ImGui::SaveIniSettingsToDisk(ini_file.data());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

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

    ImGui::Begin("Stats");
    ImGui::Text("Delta time:  %.3f", sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value());
    ImGui::Text("FPS:         %d", _fps);

    auto node = scene.root();

    _build_tree(node);

    ImGui::End();

    _build_node_preview();

    ImGui::Render();
    auto* draw_data = ImGui::GetDrawData();

    ImGui_ImplVulkan_RenderDrawData(draw_data, command_buffer);
  }

private:

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

  imgui::pipeline _pipeline;

  bool _show_demo_window;
  sbx::math::color _clear_color;

  sbx::units::second _time;
  std::uint32_t _frames;
  std::uint32_t _fps;

  sbx::math::uuid _selected_node_id;

};

} // namespace demo

#endif // DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
