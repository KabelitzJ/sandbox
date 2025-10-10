#ifndef LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_
#define LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <libsbx/utility/logger.hpp>
#include <libsbx/utility/exception.hpp>
#include <libsbx/utility/target.hpp>

#include <libsbx/memory/blob.hpp>

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

    auto preprocessor_macro_descriptions = std::array<slang::PreprocessorMacroDesc, 1u>{
      slang::PreprocessorMacroDesc{ "SBX_DEBUG", utility::is_build_configuration_debug_v ? "1" : "0" }
    };

    session_description.preprocessorMacros = preprocessor_macro_descriptions.data();
    session_description.preprocessorMacroCount = preprocessor_macro_descriptions.size();

    _global_session->createSession(session_description, _session.writeRef());
  }

  ~compiler() {

  }

  auto compile(const std::filesystem::path& path, const std::vector<define>& defines = {}) -> Slang::ComPtr<slang::IBlob> {
    const auto source = _read_file(path);

    const auto complete_source = _inject_defines(source, defines);

    auto shader_module = Slang::ComPtr<slang::IModule>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      shader_module = _session->loadModuleFromSourceString(path.filename().string().c_str(), nullptr, complete_source.c_str(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!shader_module) {
        return nullptr;
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
        return nullptr;
      }
    }

    auto linked = Slang::ComPtr<slang::IComponentType>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};

      composed->link(linked.writeRef(), diagnostic.writeRef());

      _diagnose_if_needed(diagnostic);

      if (!linked) {
        return nullptr;
      }
    }

    auto code = Slang::ComPtr<slang::IBlob>{};

    {
      auto diagnostic = Slang::ComPtr<slang::IBlob>{};
  
      linked->getEntryPointCode(0, 0, code.writeRef(), diagnostic.writeRef());
  
      _diagnose_if_needed(diagnostic);

      if (!code) {
        return nullptr;
      }
    }

    return code;
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
    if (defines.empty()) {
      return source;
    }

    auto prologue = std::string{};
    prologue.reserve(64u * defines.size());

    for (const auto& [key, value] : defines) {
      prologue += "#define " + key + (value.empty() ? "" : " " + value) + "\n";
    }

    prologue += "\n";

    return prologue + source;
  }

  Slang::ComPtr<slang::IGlobalSession> _global_session;
  Slang::ComPtr<slang::ISession> _session;

}; // class compiler

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_PIPELINE_COMPILER_HPP_
