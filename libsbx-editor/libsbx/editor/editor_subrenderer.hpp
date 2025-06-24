#ifndef LIBSBX_EDITOR_EDITOR_SUBRENDERER_HPP_
#define LIBSBX_EDITOR_EDITOR_SUBRENDERER_HPP_

#include <deque>

#include <imgui.h>
#include <imnodes.h>
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

  editor_subrenderer(const std::filesystem::path& path, const sbx::graphics::pipeline::stage& stage, const std::string& attachment_name)
  : base{stage},
    _attachment_name{attachment_name},
    _pipeline{path, stage},
    _descriptor_handler{_pipeline, 0u},
    _show_demo_window{false},
    _clear_color{sbx::math::color::black()},
    _selected_node_id{sbx::math::uuid::null()} {
    // Initialize ImGui
    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImNodes::CreateContext();
    ImPlot::CreateContext();

    auto& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = ini_file.data();

    ImGui::StyleColorsDark();
    ImNodes::StyleColorsDark();

    _setup_style();

    auto& device_module = sbx::core::engine::get_module<sbx::devices::devices_module>();
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    graphics_module.set_dynamic_size_callback([&]() -> sbx::math::vector2u {
      return _viewport_size;
    });

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
    ImNodes::DestroyContext();
    ImGui::DestroyContext();
  }

  auto render(sbx::graphics::command_buffer& command_buffer) -> void override {
    auto& graphics_module = sbx::core::engine::get_module<sbx::graphics::graphics_module>();

    _pipeline.bind(command_buffer);

    _descriptor_handler.push("sTexture", graphics_module.attachment(_attachment_name));

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

  auto _context_menu(const sbx::scenes::node node) -> void {
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

      if (_selected_node_id != scene.get_component<sbx::scenes::id>(scene.root())) {
        if (ImGui::MenuItem("Delete")) {
          _open_popups.set(popup::hierarchy_delete);
        }
      }

      ImGui::EndPopup();
    }
  }

  auto _build_tree(const sbx::scenes::node node) -> void {
    auto& scene_module = sbx::core::engine::get_module<sbx::scenes::scenes_module>();

    auto& scene = scene_module.scene();

    const auto& relationship = scene.get_component<sbx::scenes::relationship>(node);

    // auto flag = ImGuiTreeNodeFlags{ImGuiTreeNodeFlags_OpenOnArrow};
    auto flag = ImGuiTreeNodeFlags{ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow};

    if (relationship.children().empty()) {
      flag |= ImGuiTreeNodeFlags_Leaf;
    }

    if (scene.get_component<sbx::scenes::id>(node) == _selected_node_id) {
      flag |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushID(&node);

    if (ImGui::TreeNodeEx(scene.get_component<sbx::scenes::tag>(node).c_str(), flag)) {
      if (ImGui::IsItemClicked(ImGuiMouseButton_Right) || ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        _selected_node_id = scene.get_component<sbx::scenes::id>(node);

        utility::logger<"editor">::debug("Selected node id {}", _selected_node_id);
      }

      _context_menu(node);

      for (const auto& child_id : relationship.children()) {
        if (auto child = scene.find_node(child_id); child != scenes::node::null) {
          _build_tree(child);
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

    if (auto node = scene.find_node(_selected_node_id); node != scenes::node::null) {
      if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen)) {
        buffer.fill('\0');

        auto& tag = scene.get_component<sbx::scenes::tag>(node);

        std::memcpy(buffer.data(), tag.data(), std::min(tag.size(), buffer.size()));

        if (ImGui::InputText("##label", buffer.data(), buffer.size())) {
          tag = buffer.data();
        }

        ImGui::TreePop();
      }

      if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        auto& transform = scene.get_component<sbx::math::transform>(node);

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
          if (auto node = scene.find_node(_selected_node_id); node != scenes::node::null) {
            scene.create_child_node(node, name);
          } else {
            utility::logger<"editor">::warn("No selected node");
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

        if (auto node = scene.find_node(_selected_node_id); node != scenes::node::null) {
          ImGui::Text("Do you want to delete '%s'", scene.get_component<sbx::scenes::tag>(node).c_str());

          ImGui::SetCursorPosX(ImGui::GetCursorPosX() + available_width - total_width);

          const auto footer_height = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
          ImGui::SetCursorPosY(ImGui::GetWindowContentRegionMax().y - footer_height);

          if (ImGui::Button("Cancel", ImVec2{button_width, 0})) {
            ImGui::CloseCurrentPopup();
          }

          ImGui::SameLine();

          if (ImGui::Button("Delete", ImVec2{button_width, 0})) {
            scene.destroy_node(node);
            _selected_node_id = sbx::math::uuid::null();
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
      ImGui::Begin("Settings");

      auto& settings = core::engine::settings();

      settings.for_each([](const auto& group_name, auto& group) {
        if (ImGui::CollapsingHeader(group_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
          for (const auto& entry : group.entries) {
            if (std::holds_alternative<bool>(entry.value.entry)) {
              ImGui::Checkbox(entry.name.c_str(), &std::get<bool>(entry.value.entry));
            } else if (std::holds_alternative<std::uint32_t>(entry.value.entry)) {
              ImGui::DragScalar(entry.name.c_str(), ImGuiDataType_U32, &std::get<std::uint32_t>(entry.value.entry), 1.0f);
            } else if (std::holds_alternative<std::int32_t>(entry.value.entry)) {
              ImGui::DragScalar(entry.name.c_str(), ImGuiDataType_S32, &std::get<std::int32_t>(entry.value.entry), 1.0f);
            } else if (std::holds_alternative<std::float_t>(entry.value.entry)) {
              const auto* min = std::holds_alternative<std::float_t>(entry.value.min) ? &std::get<std::float_t>(entry.value.min) : nullptr;
              const auto* max = std::holds_alternative<std::float_t>(entry.value.max) ? &std::get<std::float_t>(entry.value.max) : nullptr;

              ImGui::DragScalar(entry.name.c_str(), ImGuiDataType_Float, &std::get<std::float_t>(entry.value.entry), 0.001f, min, max);
            } else if (std::holds_alternative<std::string>(entry.value.entry)) {
              auto& str = std::get<std::string>(entry.value.entry);
              ImGui::InputText(entry.name.c_str(), str.data(), str.size());
            }
          }
        }
      });

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

      if (ImGui::CollapsingHeader("Frame", ImGuiTreeNodeFlags_DefaultOpen)) {
        const auto ms = units::quantity_cast<sbx::units::millisecond>(delta_time);
        ImGui::Text("Delta time");
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Text, _get_color_for_time(ms));
        ImGui::Text("  %.3f [ms]", ms.value());
        ImGui::PopStyleColor();
        // ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, _get_color_for_time(ms));
        ImGui::ProgressBar(ms.value() / 16.66, ImVec2(200, 15));
        ImGui::PopStyleColor();

        ImGui::Text("FPS          %d", _fps);
      }

      if (ImGui::CollapsingHeader("Profiler", ImGuiTreeNodeFlags_DefaultOpen)) {
        // static auto filter_buffer = std::array<char, 64u>{};

        // // [TODO] KAJ 2025-06-03 : Filtering is not yet implemented
        // ImGui::InputTextWithHint("##profiler_filter", "Filter...", filter_buffer.data(), filter_buffer.size());

        core::engine::profiler().for_each([](const auto& group_name, const auto& group){
          if (ImGui::TreeNodeEx(group_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::BeginTable("group_table", 2, ImGuiTableFlags_SizingFixedFit)) {
              // Optional: Table header
              // ImGui::TableSetupColumn("Name");
              // ImGui::TableSetupColumn("Time (ms)");
              // ImGui::TableHeadersRow();

              // [1] Add total if available
              if (group.overall.value() > 0.0f) {
                const auto ms = units::quantity_cast<sbx::units::millisecond>(group.overall);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("total");

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, _get_color_for_time(ms));
                ImGui::Text("%.3f [ms]", ms.value());
                ImGui::PopStyleColor();
              }

              // [2] Add entries
              for (const auto& [name, measurement] : group.entries) {
                const auto ms = units::quantity_cast<sbx::units::millisecond>(measurement);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", name.c_str());

                ImGui::TableSetColumnIndex(1);
                ImGui::PushStyleColor(ImGuiCol_Text, _get_color_for_time(ms));
                ImGui::Text("%.3f [ms]", ms.value());
                ImGui::PopStyleColor();
              }

              ImGui::EndTable();
            }

            ImGui::TreePop();
          }
        });
      }

      // static constexpr auto max_time = sbx::units::second{5};

      // _elapsed += delta_time;

      // _deltas.push_back(sbx::units::quantity_cast<sbx::units::millisecond>(delta_time).value());
      // _time_stamps.push_back(_elapsed.value());

      // while (!_time_stamps.empty() && (_elapsed - sbx::units::second{_time_stamps.front()} > max_time)) {
      //   _deltas.erase(_deltas.begin());
      //   _time_stamps.erase(_time_stamps.begin());
      // }

      // if (ImPlot::BeginPlot("Delta Time Graph", ImVec2{-1, 150})) {
      //   ImPlot::SetupAxes("Time (s)", "Delta Time (ms)", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_AutoFit);
      //   ImPlot::SetupAxisLimits(ImAxis_X1, (_elapsed - max_time).value(), _elapsed.value(), ImGuiCond_Always);
      //   ImPlot::PlotShaded("Delta Time", _time_stamps.data(), _deltas.data(), _deltas.size(), 0.0f);
      //   ImPlot::EndPlot();
      // }

      ImGui::End();
    }

    {
      ImGui::Begin("Nodes");

      ImNodes::BeginNodeEditor();

      static auto links = std::vector<std::pair<std::int32_t, std::int32_t>>{};

      static auto color = ImVec4{1.0f, 0.0f, 0.0f, 1.0f};

      if (ImNodes::IsEditorHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup("editor_context_menu");
      }

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));

      if (ImGui::BeginPopup("editor_context_menu")) {
        if (ImGui::MenuItem("Color")) {
          
        }
        if (ImGui::MenuItem("Add")) {

        }
        if (ImGui::MenuItem("Subtract")) {

        }
        if (ImGui::MenuItem("Multiply")) {

        }
        if (ImGui::MenuItem("Remap")) {

        }
        ImGui::EndPopup();
      }

      ImGui::PopStyleVar();

      ImNodes::PushColorStyle(ImNodesCol_TitleBar, IM_COL32(250, 0, 0, 255));
      ImNodes::PushColorStyle(ImNodesCol_TitleBarSelected, IM_COL32(255, 0, 0, 255));
      ImNodes::PushColorStyle(ImNodesCol_TitleBarHovered, IM_COL32(255, 0, 0, 255));
      ImNodes::BeginNode(0);

      ImNodes::BeginNodeTitleBar();
      ImGui::TextUnformatted("Color");
      ImNodes::EndNodeTitleBar();

      ImNodes::BeginOutputAttribute(1);
      ImGui::SetNextItemWidth(150);
      ImGui::DragFloat("##red", &color.x, 0.001f, 0.0f, 1.0f);
      ImGui::SameLine();
      ImGui::Text("  Red");
      ImNodes::EndOutputAttribute();  

      ImNodes::BeginOutputAttribute(2);
      ImGui::SetNextItemWidth(150);
      ImGui::DragFloat("##green", &color.y, 0.001f, 0.0f, 1.0f);
      ImGui::SameLine();
      ImGui::Text("Green");
      ImNodes::EndOutputAttribute();

      ImNodes::BeginOutputAttribute(3);
      ImGui::SetNextItemWidth(150);
      ImGui::DragFloat("##blue", &color.z, 0.001f, 0.0f, 1.0f);
      ImGui::SameLine();
      ImGui::Text(" Blue");
      ImNodes::EndOutputAttribute();

      ImNodes::BeginOutputAttribute(4);
      ImGui::SetNextItemWidth(150);
      ImGui::DragFloat("##alpha", &color.w, 0.001f, 0.0f, 1.0f);
      ImGui::SameLine();
      ImGui::Text("Alpha");
      ImNodes::EndOutputAttribute();

      ImGui::Dummy(ImVec2(0.0f, 10.0f));

      ImGui::SetNextItemWidth(200);
      ImGui::ColorPicker4("##color_picker", &color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoSidePreview);

      ImNodes::EndNode();
      ImNodes::PopColorStyle();
      ImNodes::PopColorStyle();
      ImNodes::PopColorStyle();

      ImNodes::BeginNode(5);
      ImNodes::BeginNodeTitleBar();
      ImGui::TextUnformatted("add");
      ImNodes::EndNodeTitleBar();
      ImNodes::BeginInputAttribute(6);
      ImGui::Text("in");
      ImNodes::EndOutputAttribute();
      ImNodes::EndNode();

      for (auto i = 0; i < links.size(); ++i) {
        const auto link = links[i];
        ImNodes::Link(i, link.first, link.second);
      }

      ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);

      ImNodes::EndNodeEditor();

      auto start = 0;
      auto end = 0;

      if (ImNodes::IsLinkCreated(&start, &end)) {
        links.emplace_back(start, end);
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

      _viewport_size = sbx::math::vector2u{static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};

      ImGui::Text("Width: %f", width);
      ImGui::Text("Height: %f", height);
      ImGui::Text("Aspect Ratio: %f", width / height);

      _build_node_preview();

      ImGui::End();
    }

    {
      ImGui::Begin("Log");
    
      if (ImGui::Button("Clear"))  {
        utility::logger<"editor">::info("Clearing log");
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

  auto _setup_style() -> void {
    ImGui::GetStyle().FrameRounding = 4.0f;
    ImGui::GetStyle().GrabRounding = 4.0f;
    
    auto* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 0.37f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 0.16f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 0.57f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.21f, 0.27f, 0.31f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.73f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.07f, 0.10f, 0.15f, 0.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.19f, 0.41f, 0.78f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.20f, 0.25f, 0.29f, 0.80f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
    colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 0.20f);
  }

  static auto _lerp_color(const ImVec4& a, const ImVec4& b, std::float_t t) {
    return ImVec4{
      a.x + (b.x - a.x) * t,
      a.y + (b.y - a.y) * t,
      a.z + (b.z - a.z) * t,
      a.w + (b.w - a.w) * t
    };
  }

  static auto _get_color_for_time(const units::millisecond ms) -> ImVec4 {
    static constexpr auto green  = ImVec4{0.40f, 0.85f, 0.40f, 1.0f};
    static constexpr auto yellow = ImVec4{0.95f, 0.85f, 0.20f, 1.0f};
    static constexpr auto orange = ImVec4{0.95f, 0.55f, 0.15f, 1.0f};
    static constexpr auto red    = ImVec4{0.90f, 0.25f, 0.25f, 1.0f};

    if (ms <= 10.0f) {
      return green;
    } else if (ms <= 16.66f) {
      const auto t = (ms - 10.0f) / (16.66f - 10.0f);
      return _lerp_color(green, yellow, t);
    } else if (ms <= 33.33f) {
      const auto t = (ms - 16.66f) / (33.33f - 16.66f);
      return _lerp_color(yellow, red, t);
    } else {
      return red;
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

  std::string _attachment_name;

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

  math::vector2u _viewport_size;

  std::vector<std::float_t> _deltas;
  std::vector<std::float_t> _time_stamps;
  sbx::units::second _elapsed;

}; // class editor_subrenderer

} // namespace sbx::editor

#endif // LIBSBX_EDITOR_EDITOR_SUBRENDERER_HPP_
