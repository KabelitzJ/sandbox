#ifndef LIBSBX_MODELS_PIPELINE_HPP_
#define LIBSBX_MODELS_PIPELINE_HPP_

#include <fstream>

#include <nlohmann/json.hpp>

#include <libsbx/utility/enum.hpp>

#include <libsbx/graphics/pipeline/graphics_pipeline.hpp>

#include <libsbx/models/vertex3d.hpp>

namespace sbx::models {

class pipeline : public graphics::graphics_pipeline<vertex3d> {

  inline static constexpr auto pipeline_definition = graphics::pipeline_definition{
    .uses_depth = true,
    .uses_transparency = false,
    .rasterization_state = graphics::rasterization_state{
      .polygon_mode = graphics::polygon_mode::fill,
      .cull_mode = graphics::cull_mode::back,
      .front_face = graphics::front_face::counter_clockwise
    }
  };

  using base_type = graphics::graphics_pipeline<vertex3d>;

public:

  using vertex_type = vertex3d;

  pipeline(const std::filesystem::path& path, const graphics::pipeline::stage& stage)
  : base_type{path, stage, pipeline_definition} { }

  ~pipeline() override = default;

private:

  auto _load_definition(const std::filesystem::path& path) -> graphics::pipeline_definition {
    if (!std::filesystem::exists(path / "definition.json")) {
      return pipeline_definition;
    }

    auto file = std::ifstream{path / "definition.json"};

    if (!file.is_open()) {
      return pipeline_definition;
    }

    auto definition = nlohmann::json::parse(file);

    auto result = pipeline_definition;

    if (definition.contains("uses_depth")) {
      result.uses_depth = definition["uses_depth"].get<bool>();
    }

    if (definition.contains("uses_transparency")) {
      result.uses_depth = definition["uses_transparency"].get<bool>();
    }

    if (definition.contains("rasterization_state")) {
      auto rasterization_state = definition["rasterization_state"];

      if (rasterization_state.contains("polygon_mode")) {
        auto polygon_mode = utility::from_string<graphics::polygon_mode>(rasterization_state["polygon_mode"].get<std::string>());

        if (polygon_mode) {
          result.rasterization_state.polygon_mode = *polygon_mode;
        } else {
          core::logger::warn("Could not parse 'sbx::graphics::polygon_mode' value '{}'", rasterization_state["polygon_mode"].get<std::string>());
        }
      }
    }

    return result;
  }

}; // class pipeline

} // namespace sbx::models

#endif // LIBSBX_MODELS_PIPELINE_HPP_
