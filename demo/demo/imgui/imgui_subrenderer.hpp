#ifndef DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
#define DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_

#include <imgui.h>
#include <demo/imgui/bindings/1.89.7-docking/imgui_impl_glfw.h>
#include <demo/imgui/bindings/1.89.7-docking/imgui_impl_vulkan.h>

#include <libsbx/devices/devices_module.hpp>

#include <libsbx/graphics/subrenderer.hpp>
#include <libsbx/graphics/graphics_module.hpp>

#include <demo/imgui/pipeline.hpp>

namespace demo {

class imgui_subrenderer final : public sbx::graphics::subrenderer {

  inline static constexpr auto ini_file = std::string_view{"demo/assets/data/imgui.ini"};

  inline static constexpr auto max_log_lines = 100u;

  using base = sbx::graphics::subrenderer;

public:

  imgui_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage)
  : base{stage},
    _pipeline{path, stage},
    _show_demo_window{false},
    _clear_color{sbx::math::color::black} {
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

    auto& window = device_module.window();

    // Connect ImGui to GLFW
    ImGui_ImplGlfw_InitForVulkan(window, true);

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

    // ImGui::LoadIniSettingsFromDisk(ini_file.data());
  }

  ~imgui_subrenderer() override {
    // ImGui::SaveIniSettingsToDisk(ini_file.data());

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

  auto _split(const std::string& string, const std::string& delimiter) -> std::vector<std::string> {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = string.find(delimiter, pos_start)) != std::string::npos) {
      token = string.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
    }

    if (pos_start != 0u) {
      res.push_back(string.substr(pos_start));
    }
    return res;
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

      ImGui::Image(_descriptor_handler.descriptor_set(), available_size);

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
  bool _has_auto_scroll;

}; // class imgui_subrenderer

} // namespace demo

#endif // DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
