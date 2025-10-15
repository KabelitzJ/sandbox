#include <libsbx/graphics/pipeline/compiler.hpp>

#include <libsbx/core/engine.hpp>

#include <libsbx/assets/assets_module.hpp>

#include <fstream>

namespace sbx::graphics {

#define DUMP 0

compiler::compiler() {
  createGlobalSession(_global_session.writeRef());
}

compiler::~compiler() {

}

auto compiler::compile(const compile_request& compile_request) -> std::vector<std::uint32_t> {
  // [NOTE] KAJ 2025-10-15: We need to lazy initialize because asset root is not known when the compiler is created
  if (!_session) {
    _initialize_session();
  }

  auto& assets_module = core::engine::get_module<assets::assets_module>();

  const auto resolved_path = assets_module.resolve_path(compile_request.path);

  const auto source = _read_file(resolved_path);

  if (source.empty()) {
    throw std::runtime_error{"source empty"};
  }

  auto request = Slang::ComPtr<slang::ICompileRequest>{};

  if (SLANG_FAILED(_session->createCompileRequest(request.writeRef()))) {
    utility::logger<"graphics">::error("createCompileRequest failed");
    throw std::runtime_error{"createCompileRequest failed"};
  }

  const auto target_index = request->addCodeGenTarget(SLANG_SPIRV);

  request->setTargetProfile(target_index, _global_session->findProfile("spirv_1_5"));

  request->addTargetCapability(target_index, _global_session->findCapability("spirv_1_5"));
  request->addTargetCapability(target_index, _global_session->findCapability("SPV_EXT_physical_storage_buffer"));

  request->setTargetFlags(target_index, SLANG_TARGET_FLAG_GENERATE_SPIRV_DIRECTLY);

  request->setMatrixLayoutMode(SLANG_MATRIX_LAYOUT_COLUMN_MAJOR);
  request->setDebugInfoLevel(SLANG_DEBUG_INFO_LEVEL_NONE);
  request->setOptimizationLevel(SLANG_OPTIMIZATION_LEVEL_NONE);

  for (const auto& define : compile_request.defines) {
    request->addPreprocessorDefine(define.key.c_str(), define.value.c_str());
  }

  request->addPreprocessorDefine("SBX_DEBUG", utility::is_build_configuration_debug_v ? "1" : "0");

  const auto parent_dir = resolved_path.parent_path();
  request->addSearchPath(parent_dir.string().c_str());

  for (const auto& include : compile_request.includes) {
    request->addSearchPath(include.string().c_str());
  }

  const auto translation_unit = request->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, resolved_path.filename().string().c_str());

  request->addTranslationUnitSourceString(translation_unit, resolved_path.string().c_str(), source.c_str());

  const auto entry_point = request->addEntryPoint(translation_unit, compile_request.entry_point.c_str(), compile_request.stage);

  if (SLANG_FAILED(request->compile())) {
    if (const auto* diagnostic = request->getDiagnosticOutput()) {
      utility::logger<"graphics">::error("{}", diagnostic);
    }

    throw std::runtime_error{"compile failed"};
  }

  auto code = Slang::ComPtr<slang::IBlob>{};

  if (SLANG_FAILED(request->getEntryPointCodeBlob(entry_point, target_index, code.writeRef()))) {
    if (const char* diagnostic = request->getDiagnosticOutput()) {
      utility::logger<"graphics">::error("{}", diagnostic);
    }

    throw std::runtime_error{"get code failed"};
  }

  const auto* words = static_cast<const std::uint32_t*>(code->getBufferPointer());
  const auto byte_count = code->getBufferSize();
  const auto word_count = byte_count / sizeof(std::uint32_t);

  return std::vector<std::uint32_t>{words, words + word_count};
}

auto compiler::_read_file(const std::filesystem::path& path) -> std::string {
  auto file = std::ifstream{path, std::ios::binary};

  if (!file) {
    throw utility::runtime_error{"File does not exist {}", path.string()};
  }

  return std::string{std::istreambuf_iterator<char>{file}, std::istreambuf_iterator<char>{}};
}

auto compiler::_initialize_session() -> void {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

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
    slang::CompilerOptionEntry{ slang::CompilerOptionName::DebugInformation, { slang::CompilerOptionValueKind::Int, SLANG_DEBUG_INFO_LEVEL_NONE, 0, nullptr, nullptr } },
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

  auto search_paths = std::array<const char*, 1u>{
    std::filesystem::path{assets_module.asset_root()}.append("shaders").c_str()
  };

  session_description.searchPaths = search_paths.data();
  session_description.searchPathCount = search_paths.size();

  _global_session->createSession(session_description, _session.writeRef());
}

} // namespace sbx::graphics
