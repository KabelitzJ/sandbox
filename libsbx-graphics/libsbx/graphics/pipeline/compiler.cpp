#include <libsbx/graphics/pipeline/compiler.hpp>

#include <fstream>

namespace sbx::graphics {

compiler::compiler() {
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

compiler::~compiler() {

}

auto compiler::compile(const compile_request& compile_request) -> std::vector<std::uint32_t> {
  const auto source = _read_file(compile_request.path);

  if (source.empty()) {
    return {};
  }

  auto request = Slang::ComPtr<slang::ICompileRequest>{};

  if (SLANG_FAILED(_session->createCompileRequest(request.writeRef()))) {
    utility::logger<"graphics">::error("createCompileRequest failed");
    return {};
  }

  const auto target_index = request->addCodeGenTarget(SLANG_SPIRV);

  request->setTargetProfile(target_index, _global_session->findProfile("spirv_1_5"));

  request->addTargetCapability(target_index, _global_session->findCapability("spirv_1_5"));
  request->addTargetCapability(target_index, _global_session->findCapability("SPV_EXT_physical_storage_buffer"));

  request->setTargetFlags(target_index, SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY);

  request->setMatrixLayoutMode(SLANG_MATRIX_LAYOUT_COLUMN_MAJOR);
  request->setDebugInfoLevel(SLANG_DEBUG_INFO_LEVEL_STANDARD);
  request->setOptimizationLevel(SLANG_OPTIMIZATION_LEVEL_NONE);

  for (const auto& define : compile_request.defines) {
    request->addPreprocessorDefine(define.key.c_str(), define.value.c_str());
  }

  request->addPreprocessorDefine("SBX_DEBUG", utility::is_build_configuration_debug_v ? "1" : "0");

  const auto parent_dir = compile_request.path.parent_path();
  request->addSearchPath(parent_dir.string().c_str());

  for (const auto& include : compile_request.includes) {
    request->addSearchPath(include.string().c_str());
  }

  const auto translation_unit = request->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, compile_request.path.filename().string().c_str());

  request->addTranslationUnitSourceString(translation_unit, compile_request.path.string().c_str(), source.c_str());

  const auto entry_point = request->addEntryPoint(translation_unit, compile_request.entry_point.c_str(), compile_request.stage);

  if (SLANG_FAILED(request->compile())) {
    if (const auto* diagnostic = request->getDiagnosticOutput()) {
      utility::logger<"graphics">::error("{}", diagnostic);
    }

    return {};
  }

  auto code = Slang::ComPtr<slang::IBlob>{};

  if (SLANG_FAILED(request->getEntryPointCodeBlob(entry_point, target_index, code.writeRef()))) {
    if (const char* diagnostic = request->getDiagnosticOutput()) {
      utility::logger<"graphics">::error("{}", diagnostic);
    }

    return {};
  }

  return std::vector<std::uint32_t>{ static_cast<const std::uint32_t*>(code->getBufferPointer()), static_cast<const std::uint32_t*>(code->getBufferPointer()) + code->getBufferSize() };
}

auto compiler::_read_file(const std::filesystem::path& path) -> std::string {
  auto file = std::ifstream{path, std::ios::binary};

  if (!file) {
    return std::string{};
  }

  return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

auto compiler::_inject_defines(const std::string& source, const std::vector<define>& defines) -> std::string {
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

} // namespace sbx::graphics
