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

    _io = &ImGui::GetIO();
    _io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    _io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    _io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

    ImGui::LoadIniSettingsFromDisk(ini_file.data());

    // window.on_framebuffer_resized() += [&]([[maybe_unused]] const sbx::devices::framebuffer_resized_event& event) {
    //   const auto& scene_image = dynamic_cast<const sbx::graphics::image2d&>(graphics_module.attachment("scene"));

    //   // for (const auto& descriptor_set : _descriptor_sets) {
    //   //   ImGui_ImplVulkan_RemoveTexture(descriptor_set);
    //   // }

    //   _descriptor_sets.resize(graphics_module.swapchain().image_count());

    //   for (auto i = 0; i < graphics_module.swapchain().image_count(); ++i) {
    //     _descriptor_sets[i] = ImGui_ImplVulkan_AddTexture(scene_image.sampler(), scene_image.view(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    //   }
    // };

    // [TODO] KAJ 2024-12-02 : Fix resizing

    const auto& scene_image = dynamic_cast<const sbx::graphics::image2d&>(graphics_module.attachment("scene"));
    
    _descriptor_sets.resize(3);

    for (auto i = 0; i < 3; ++i) {
      _descriptor_sets[i] = ImGui_ImplVulkan_AddTexture(scene_image.sampler(), scene_image.view(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    _cout = std::cout.rdbuf();
    std::cout.rdbuf(_log.rdbuf());
  }

  ~imgui_subrenderer() override {
    std::cout.rdbuf(_cout);

    ImGui::SaveIniSettingsToDisk(ini_file.data());

    for (const auto& descriptor_set : _descriptor_sets) {
      ImGui_ImplVulkan_RemoveTexture(descriptor_set);
    }

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
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
    ImGui::Begin("Hierarchy");
    ImGui::Text("Hello, world!");
    if (ImGui::Button("Demo Window")) {
      sbx::core::logger::info("Button pressed");
      std::cout << "Button pressed" << std::endl;
    }
    ImGui::End();

    // Example Window 2
    ImGui::Begin("Scene");
    auto current_frame = graphics_module.current_frame();
    auto available_size = ImGui::GetContentRegionAvail();
    auto descriptor_set = _descriptor_sets[current_frame];
    ImGui::Image(descriptor_set, available_size);
    if (ImGui::IsItemHovered()) {
      ImGuiIO& io = ImGui::GetIO();
      io.WantCaptureMouse = false; 
      io.WantCaptureKeyboard = false;
    }
    ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    ImVec2 vMax = ImGui::GetWindowContentRegionMax();

    vMin.x += ImGui::GetWindowPos().x;
    vMin.y += ImGui::GetWindowPos().y;
    vMax.x += ImGui::GetWindowPos().x;
    vMax.y += ImGui::GetWindowPos().y;
    ImGui::End();

    // Example Window 3
    ImGui::Begin("Properties");
    auto width = vMax.x - vMin.x;
    auto height = vMax.y - vMin.y;
    ImGui::Text("Width: %f", width);
    ImGui::Text("Height: %f", height);
    ImGui::End();

    // Example Window 4
    ImGui::Begin("Log");
    if (ImGui::Button("Clear"))  {
      // _clear_console();
      _logs.clear();
    }

    ImGui::SameLine();
    ImGui::Checkbox("Auto-Scroll", &_has_auto_scroll);

    auto lines = _split(_log.str(), "\n");
    for (const auto& line : lines) {
      _logs.push_front(line);
    }

    while (_logs.size() > max_log_lines) {
      _logs.pop_back();
    }

    _log.str(std::string{});
    _log.clear();

    ImGui::BeginChild("ScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
    for (const auto& log : _logs) {
      ImGui::TextUnformatted(log.c_str());
    }

    if (_has_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
      ImGui::SetScrollHereY(0.999f);
    }

    ImGui::EndChild();
    ImGui::End();
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

  ImGuiIO* _io;

  bool _show_demo_window;
  sbx::math::color _clear_color;

  std::vector<VkDescriptorSet> _descriptor_sets;

  bool _has_auto_scroll;
  std::deque<std::string> _logs;
  std::ostringstream _log;
  std::streambuf* _cout;

};

} // namespace demo

#endif // DEMO_IMGUI_IMGUI_SUBRENDERER_HPP_
