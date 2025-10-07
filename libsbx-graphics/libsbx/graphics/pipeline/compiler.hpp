#ifndef LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>

namespace sbx::graphics {

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

  auto compile(const std::filesystem::path& path, const std::filesystem::path& output, const std::vector<define>& defines = {}) -> bool {
    auto source = _read_file(path);

    source = _inject_defines(source, defines);

    auto shader_module = Slang::ComPtr<slang::IModule>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      shader_module = _session->loadModuleFromSourceString(path.filename().c_str(), nullptr, source.c_str(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!shader_module) {
        return false;
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

      _session->createCompositeComponentType(components.data(), static_cast<SlangInt>(components.size()), composed.writeRef(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!composed) {
        return false;
      }
    }

    auto linked = Slang::ComPtr<slang::IComponentType>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      composed->link(linked.writeRef(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!linked) {
        return false;
      }
    }

    auto code = Slang::ComPtr<slang::IBlob>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};
  
      linked->getEntryPointCode(0, 0, code.writeRef(), diagnostic.writeRef());
  
      _diagnose_if_needed(diagnostic);

      if (!code) {
        return false;
      }
    }

    if (!_write_spv_to_file(output, code)) {
      return false;
    }

    return true;
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

    out.write(reinterpret_cast<const char*>(blob->getBufferPointer()), static_cast<std::streamsize>(blob->getBufferSize()));
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