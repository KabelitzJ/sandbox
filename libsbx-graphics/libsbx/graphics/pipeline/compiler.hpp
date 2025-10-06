#ifndef LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

namespace sbx::graphics {

class program {

  friend class compiler;

public:

  struct entry_point_info {
    SlangStage stage;
    std::string name;
  }; // struct entry_point_info

  struct reflected_binding {
    std::uint32_t set = 0;
    std::uint32_t binding = 0;
    std::uint32_t count = 1;
    std::string name;

    slang::ParameterCategory category = slang::ParameterCategory::DescriptorTableSlot;
    slang::TypeReflection* type = nullptr;
    slang::TypeLayoutReflection* typeLayout = nullptr;
  }; // struct reflected_binding

  auto layout() const -> slang::ProgramLayout* { 
    return _layout; 
  }

  auto entry_points() const -> std::vector<entry_point_info> {
    auto entry_points = std::vector<entry_point_info>{};

    const auto count = _layout->getEntryPointCount();

    entry_points.reserve(count);

    for (auto i = 0; i < count; ++i) {
      auto entry_point = _layout->getEntryPointByIndex(i);

      auto info = entry_point_info{};
      info.stage = entry_point->getStage();
      info.name  = entry_point->getName() ? entry_point->getName() : "";

      entry_points.push_back(std::move(info));
    }

    return entry_points;
  }

  auto bindings() const -> std::vector<reflected_binding> {
    auto out = std::vector<reflected_binding>{};

    _collect_scope_bindings(_layout->getGlobalParamsVarLayout(), out);

    const auto count = _layout->getEntryPointCount();

    for (auto i = 0; i < count; ++i) {
      auto* entry_point = _layout->getEntryPointByIndex(i);

      _collect_scope_bindings(entry_point->getVarLayout(), out);
    }

    return out;
  }

  auto push_constant_size() const -> std::size_t {
    auto* globals = _layout->getGlobalParamsVarLayout();

    if (!globals) {
      return 0;
    }

    auto* type_layout = globals->getTypeLayout();

    if (!type_layout) {
      return 0;
    }

    return type_layout->getSize(slang::ParameterCategory::PushConstantBuffer);
  }

private:

  program(Slang::ComPtr<slang::IComponentType>& program)
  : _program{program}, 
    _layout{_program->getLayout(0)} { }

  static auto _collect_scope_bindings(slang::VariableLayoutReflection* scope_var_layout, std::vector<reflected_binding>& out) -> void {
    if (!scope_var_layout) {
      return;
    }

    auto* type_layout = scope_var_layout->getTypeLayout();

    if (!type_layout) {
      return;
    }

    if (type_layout->getKind() == slang::TypeReflection::Kind::Struct) {
      const auto paramCount = type_layout->getFieldCount();

      for (auto i = 0; i < paramCount; ++i) {
        auto* param = type_layout->getFieldByIndex(i);

        _walk_type_layout(param, out, "");
      }
    } else {
      _walk_type_layout(scope_var_layout, out, "");
    }
  }

  static auto _walk_type_layout(slang::VariableLayoutReflection* variable, std::vector<reflected_binding>& out, std::string prefix = "") -> void {
    auto* type_layout = variable->getTypeLayout();
    auto* type = type_layout->getType();

    const auto name = prefix.empty() ? (variable->getVariable() && variable->getVariable()->getName() ? variable->getVariable()->getName() : "") : prefix;

    const auto binding_offset = variable->getOffset(slang::ParameterCategory::DescriptorTableSlot);

    const auto set_space = variable->getBindingSpace(slang::ParameterCategory::SubElementRegisterSpace);

    const auto binding_size = type_layout->getSize(slang::ParameterCategory::DescriptorTableSlot);

    if (binding_size > 0) {
      auto binding = reflected_binding{};
      binding.set = (uint32_t)set_space;
      binding.binding = (uint32_t)binding_offset;
      binding.count = 1;

      if (type->getKind() == slang::TypeReflection::Kind::Array) {
        const auto element_count = type->getElementCount();

        if (element_count != ~size_t(0)) {
          binding.count = (uint32_t)element_count;
        }
      }

      binding.name = name;
      binding.category = slang::ParameterCategory::DescriptorTableSlot;
      binding.type = type;
      binding.typeLayout = type_layout;

      out.push_back(std::move(binding));
    }

    switch (type->getKind()) {
      case slang::TypeReflection::Kind::Struct: {
        const int field_count = type_layout->getFieldCount();

        for (auto i = 0; i < field_count; ++i) {
          auto* field_vl = type_layout->getFieldByIndex(i);

          auto child_name = name.empty() ? (field_vl->getVariable()->getName() ? field_vl->getVariable()->getName() : "") : (name + "." + (field_vl->getVariable()->getName() ? field_vl->getVariable()->getName() : ""));

          _walk_type_layout(field_vl, out, std::move(child_name));
        }

        break;
      }
      case slang::TypeReflection::Kind::ConstantBuffer:
      case slang::TypeReflection::Kind::ParameterBlock:
      case slang::TypeReflection::Kind::TextureBuffer:
      case slang::TypeReflection::Kind::ShaderStorageBuffer: {
        if (auto* element = type_layout->getElementVarLayout(); element) {
          _walk_type_layout(element, out, name);
        }

        break;
      }
      default: {
        break;
      }
    }
  }

  static auto _space(slang::VariableLayoutReflection* variable) -> std::uint32_t {
    return variable->getBindingSpace(slang::ParameterCategory::SubElementRegisterSpace);
  }

  static auto _binding(slang::VariableLayoutReflection* variable) -> std::uint32_t {
    return variable->getOffset(slang::ParameterCategory::DescriptorTableSlot);
  }

  Slang::ComPtr<slang::IComponentType> _program;
  slang::ProgramLayout* _layout;

};

class compiler {

public:

  struct define {
    std::string key;
    std::string value;
  }; // struct define

  compiler() {
    createGlobalSession(_global_session.writeRef());

    auto session_description = slang::SessionDesc{};

    auto target_description = slang::TargetDesc{};
    target_description.format = SLANG_SPIRV;
    target_description.profile = _global_session->findProfile("spirv_1_5");

    session_description.targets = &target_description;
    session_description.targetCount = 1u;

    auto compiler_options = std::array<slang::CompilerOptionEntry, 6u>{
      slang::CompilerOptionEntry{ slang::CompilerOptionName::Capability, { slang::CompilerOptionValueKind::String, 0, 0, "spirv_1_5", nullptr } },
      slang::CompilerOptionEntry{ slang::CompilerOptionName::Capability, { slang::CompilerOptionValueKind::String, 0, 0, "SPV_EXT_physical_storage_buffer", nullptr } },
      slang::CompilerOptionEntry{ slang::CompilerOptionName::MatrixLayoutColumn, { slang::CompilerOptionValueKind::Int, 1, 0, "column_major", nullptr } },
      slang::CompilerOptionEntry{ slang::CompilerOptionName::DebugInformation, { slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr } },
      slang::CompilerOptionEntry{ slang::CompilerOptionName::Optimization, { slang::CompilerOptionValueKind::Int, 0, 0, nullptr, nullptr } },
      slang::CompilerOptionEntry{ slang::CompilerOptionName::EmitSpirvDirectly, { slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr } }
    };

    session_description.compilerOptionEntries = compiler_options.data();
    session_description.compilerOptionEntryCount = compiler_options.size();

    auto preprocessor_macro_descriptions = std::array<slang::PreprocessorMacroDesc, 2>{
      slang::PreprocessorMacroDesc{ "BIAS_VALUE", "1138" },
      slang::PreprocessorMacroDesc{ "OTHER_MACRO", "float" }
    };

    session_description.preprocessorMacros = preprocessor_macro_descriptions.data();
    session_description.preprocessorMacroCount = preprocessor_macro_descriptions.size();

    _global_session->createSession(session_description, _session.writeRef());
  }

  ~compiler() {

  }

  auto compile(const std::filesystem::path& path, const std::filesystem::path& output, const std::vector<define>& defines = {}) -> std::optional<program> {
    auto source = _read_file(path);

    source = _inject_defines(source, defines);

    auto shader_module = Slang::ComPtr<slang::IModule>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      shader_module = _session->loadModuleFromSourceString("fsModule", "shader_fs.slang", source.c_str(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!shader_module) {
        return std::nullopt;
      }
    }

    auto entry_point = Slang::ComPtr<slang::IEntryPoint>{};

    shader_module->findEntryPointByName("main", entry_point.writeRef());

    auto components = std::vector<slang::IComponentType*>{
      shader_module.get(), entry_point.get()
    };

    auto composed = Slang::ComPtr<slang::IComponentType>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      _session->createCompositeComponentType(components.data(), (SlangInt)components.size(), composed.writeRef(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!composed) {
        return std::nullopt;
      }
    }

    auto linked = Slang::ComPtr<slang::IComponentType>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      composed->link(linked.writeRef(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!linked) {
        return std::nullopt;
      }
    }

    auto code = Slang::ComPtr<slang::IBlob>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};
  
      linked->getEntryPointCode(/*entryPointIndex*/ 0, /*targetIndex*/ 0, code.writeRef(), diagnostic.writeRef());
  
      _diagnose_if_needed(diagnostic);

      if (!code) {
        return std::nullopt;
      }
    }

    if (!_write_spv_to_file(output, code)) {
      return std::nullopt;
    }

    return program{linked};
  }

private:

  static auto _diagnose_if_needed(slang::IBlob* diagnostic) -> void {
    if (diagnostic && diagnostic->getBufferSize()) {
      utility::logger<"graphics">::error("{}", diagnostic->getBufferPointer());
    }
  }

  static auto _read_file(const std::filesystem::path& path) -> std::string {
    auto file = std::ifstream{path, std::ios::binary};

    if (!file) {
      return std::string{};
    }

    return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
  }

  static auto _inject_defines(const std::string& source, const std::vector<define>& defines) -> std::string {
    auto prologue = std::string{};
    prologue.reserve(64u * defines.size());

    for (const auto& [key, value] : defines) {
      prologue += "#define " + key + (value.empty() ? "" : " " + value) + "\n";
    }

    prologue += "\n";

    return prologue + source;
  }

  static auto _write_spv_to_file(const std::filesystem::path& path, slang::IBlob* blob) -> bool {
    if (!blob || blob->getBufferSize() == 0) {
      utility::logger<"graphics">::error("Empty or null SPIR-V blob");

      return false;
    }

    auto out = std::ofstream{path, std::ios::binary};

    if (!out) {
      utility::logger<"graphics">::error("Failed to open file for writing: {}", path.string());

      return false;
    }

    out.write(reinterpret_cast<const char*>(blob->getBufferPointer()), blob->getBufferSize());
    out.close();

    if (!out) {
      utility::logger<"graphics">::error("Failed to write to file: {}", path.string());

      return false;
    }

    return true;
  }

  Slang::ComPtr<slang::IGlobalSession> _global_session;
  Slang::ComPtr<slang::ISession> _session;

}; // class compiler

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_