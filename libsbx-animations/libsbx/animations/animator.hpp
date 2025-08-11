#ifndef LIBSBX_ANIMATIONS_ANIMATOR_HPP_
#define LIBSBX_ANIMATIONS_ANIMATOR_HPP_

#include <functional>
#include <utility>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

#include <libsbx/utility/hashed_string.hpp>
#include <libsbx/utility/iterator.hpp>

#include <libsbx/math/vector3.hpp>
#include <libsbx/math/quaternion.hpp>
#include <libsbx/math/matrix4x4.hpp>
#include <libsbx/math/matrix_cast.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::animations {

struct bone_transform {
  math::vector3 position{math::vector3::zero};
  math::quaternion rotation{math::quaternion::identity};
  math::vector3 scale{math::vector3::one};
}; // struct bone_transform

inline auto lerp_bone_transform(const bone_transform& a, const bone_transform& b, const std::float_t time) -> bone_transform {
  return bone_transform{
    math::vector3::lerp(a.position, b.position, time),
    math::quaternion::slerp(a.rotation, b.rotation,time ),
    math::vector3::lerp(a.scale, b.scale, time)
  };
}

inline auto sample_clip_locals(const skeleton& skeleton, const math::uuid& animation_id, const std::float_t time) -> std::vector<bone_transform> {
  auto& assets_module = core::engine::get_module<assets::assets_module>();

  auto& animation = assets_module.get_asset<animations::animation>(animation_id);

  auto locals = utility::make_vector<bone_transform>(skeleton.bone_count());

  const auto& bones = skeleton.bones();
  const auto& tracks = animation.track_map();

  for (std::uint32_t i = 0; i < skeleton.bone_count(); ++i) {
    const auto bone_name = skeleton.name_for_bone(i);

    auto entry = tracks.find(utility::hashed_string{bone_name});

    auto transform = bone_transform{};

    transform.rotation = math::quaternion::identity;
    transform.scale = {1,1,1};

    if (entry != tracks.end()) {
      const auto& track = entry->second;

      transform.position = track.position_spline.sample(time);
      transform.rotation = math::quaternion::normalized(track.rotation_spline.sample(time));
      transform.scale = track.scale_spline.sample(time);
    }

    locals[i] = transform;
  }

  return locals;
}

inline auto locals_to_final_matrices(const skeleton& skeleton, const std::vector<bone_transform>& local_transforms, const math::matrix4x4& inverse_root_transform) -> std::vector<math::matrix4x4> {
  const auto bone_count = skeleton.bone_count();

  auto global_matrices = utility::make_vector<math::matrix4x4>(bone_count, math::matrix4x4::identity);
  auto final_matrices = utility::make_vector<math::matrix4x4>(bone_count, math::matrix4x4::identity);

  const auto& bones = skeleton.bones();

  for (std::uint32_t bone_index = 0; bone_index < bone_count; ++bone_index) {
    const auto& transform = local_transforms[bone_index];

    const auto translation_matrix = math::matrix4x4::translated(math::matrix4x4::identity, transform.position);
    const auto rotation_matrix = math::matrix_cast<4, 4>(transform.rotation);
    const auto scale_matrix = math::matrix4x4::scaled(math::matrix4x4::identity, transform.scale);

    const auto local_matrix = translation_matrix * rotation_matrix * scale_matrix;

    const auto global_matrix = (bones[bone_index].parent_id != skeleton::bone::null) ? global_matrices[bones[bone_index].parent_id] * local_matrix : local_matrix;

    global_matrices[bone_index] = global_matrix;
    final_matrices[bone_index] = inverse_root_transform * global_matrix * bones[bone_index].inverse_bind_matrix;
  }

  return final_matrices;
}

class animator {

public:

  struct state {
    utility::hashed_string name;
    math::uuid animation_id{};
    bool is_looping = true;
    std::float_t speed = 1.0f;
  }; // struct state

  struct transition {
    utility::hashed_string from;
    utility::hashed_string to;
    std::float_t duration = 0.25f;
    std::function<bool(const animator&)> condition;
    bool has_exit_time = false;
    std::float_t exit_time_normalized = 0.0f;
  }; // struct transition

  struct parameters {
    std::unordered_map<utility::hashed_string, bool> bool_values;
    std::unordered_map<utility::hashed_string, std::float_t> float_values;
    std::unordered_map<utility::hashed_string, bool> trigger_values;
  }; // struct parameters

public:

  auto add_state(const state& new_state) -> void {
    _state_map[new_state.name] = new_state;
  }

  auto add_transition(transition&& new_transition) -> void {
    _transitions.push_back(std::move(new_transition));
  }

  auto set_bool(const utility::hashed_string& key, bool value) -> void { 
    _parameters.bool_values[key] = value; 
  }

  auto set_float(const utility::hashed_string& key, std::float_t value) -> void { 
    _parameters.float_values[key] = value; 
  }

  auto set_trigger(const utility::hashed_string& key) -> void { 
    _parameters.trigger_values[key] = true; 
  }

  auto reset_trigger(const utility::hashed_string& key) -> void { 
    _parameters.trigger_values[key] = false; 
  }

  auto play(const utility::hashed_string& state_name, const bool instant = false, const std::float_t cross_fade = 0.0f) -> void {
    const auto entry = _state_map.find(state_name);

    if (entry == _state_map.end()) {
      return;
    }

    _start_playback(entry->second, instant, cross_fade);
  }

  // void play_by_animation_id(const math::uuid& animation_id, bool is_looping, std::float_t speed, bool instant = false, std::float_t cross_fade = 0.0f) {
  //   state anonymous_state{};
  //   anonymous_state.name = utility::hashed_string{"__anonymous__"};
  //   anonymous_state.animation_id = animation_id;
  //   anonymous_state.is_looping = is_looping;
  //   anonymous_state.speed = speed;
  //   _start_playback(anonymous_state, instant, cross_fade);
  // }

  void update(const std::float_t delta_time) {
    if (!_has_valid_clip(_current_state)) {
      return;
    }

    _current_state_time += delta_time * _current_state.speed;
    _current_state_time = _wrap_state_time(_current_state, _current_state_time);

    if (_is_in_transition && _has_valid_clip(_next_state)) {
      _next_state_time += delta_time * _next_state.speed;
      _next_state_time = _wrap_state_time(_next_state, _next_state_time);

      _cross_fade_elapsed = std::min(_cross_fade_elapsed + delta_time, _cross_fade_duration);

      if (_cross_fade_elapsed >= _cross_fade_duration) {
        _current_state = _next_state;
        _current_state_time = _next_state_time;
        _is_in_transition = false;
        _next_state = {};
        _next_state_time = 0.0f;
      }
    } else {
      for (const auto& rule : _transitions) {
        if (rule.from != _current_state.name) {
          continue;
        }

        if (rule.condition && !rule.condition(*this)) {
          continue;
        }

        if (rule.has_exit_time) {
          const std::float_t normalized_time = get_normalized_time(_current_state, _current_state_time);

          if (normalized_time + 1e-6f < rule.exit_time_normalized) {
            continue;
          }
        }

        const auto entry = _state_map.find(rule.to);

        if (entry == _state_map.end()) {
          continue;
        }

        _next_state = entry->second;
        _next_state_time = 0.0f;
        _is_in_transition = (rule.duration > 0.0f);
        _cross_fade_duration = std::max(0.0f, rule.duration);
        _cross_fade_elapsed = 0.0f;

        if (!_is_in_transition) {
          _current_state = _next_state;
          _current_state_time = 0.0f;
          _next_state = {};
        }

        break;
      }
    }

    for (auto& [key, value] : _parameters.trigger_values) {
      value = false;
    }
  }

  auto evaluate(const skeleton& skeleton, const math::matrix4x4& inverse_root_transform) -> std::vector<math::matrix4x4> {
    if (!_has_valid_clip(_current_state)) {
      return utility::make_vector<math::matrix4x4>(skeleton.bone_count(), math::matrix4x4::identity);
    }

    if (_is_in_transition && _has_valid_clip(_next_state) && _cross_fade_duration > 0.0f) {
      const auto blend_factor = std::clamp(_cross_fade_elapsed / _cross_fade_duration, 0.0f, 1.0f);

      auto current_locals = sample_clip_locals(skeleton, _current_state.animation_id, _current_state_time);
      auto next_locals = sample_clip_locals(skeleton, _next_state.animation_id, _next_state_time);

      auto blended_locals = utility::make_vector<bone_transform>(current_locals.size());

      for (size_t i = 0; i < blended_locals.size(); ++i) {
        blended_locals[i] = lerp_bone_transform(current_locals[i], next_locals[i], blend_factor);
      }

      return locals_to_final_matrices(skeleton, blended_locals, inverse_root_transform);
    } else {
      auto current_locals = sample_clip_locals(skeleton, _current_state.animation_id, _current_state_time);
      return locals_to_final_matrices(skeleton, current_locals, inverse_root_transform);
    }
  }

  auto get_normalized_time(const state& state, const std::float_t time) const -> std::float_t {
    const auto duration = _get_clip_duration(state.animation_id);

    if (duration <= 1e-6f) {
      return 0.0f;
    }

    return std::fmod(std::max(time, 0.0f), duration) / duration;
  }

  auto get_current_state_name() const -> const utility::hashed_string& { 
    return _current_state.name; 
  }

  auto get_is_in_transition() const -> bool { 
    return _is_in_transition;
  }

  auto get_parameters() const -> const parameters& { 
    return _parameters; 
  }

private:

  auto _start_playback(const state& target_state, const bool instant, const std::float_t cross_fade) -> void {
    if (instant || !_has_valid_clip(_current_state) || cross_fade <= 0.0f) {
      _current_state = target_state;
      _current_state_time = 0.0f;
      _is_in_transition = false;
    } else {
      _next_state = target_state;
      _next_state_time = 0.0f;
      _is_in_transition = true;
      _cross_fade_duration = cross_fade;
      _cross_fade_elapsed = 0.0f;
    }
  }

  static auto _get_clip_duration(const math::uuid& animation_id) ->  std::float_t {
    auto& assets_module = core::engine::get_module<assets::assets_module>();

    const auto& clip = assets_module.get_asset<animations::animation>(animation_id);

    return clip.duration();
  }

  auto _wrap_state_time(const state& state, std::float_t time) const -> std::float_t {
    const auto duration = _get_clip_duration(state.animation_id);

    if (duration <= 1e-6f) {
      return 0.0f;
    }

    if (!state.is_looping) {
      return std::clamp(time, 0.0f, duration);
    }

    auto wrapped = std::fmod(time, duration);

    return wrapped < 0.0f ? wrapped + duration : wrapped;
}

  auto _has_valid_clip(const state& state) const -> bool {
    return state.animation_id != math::uuid::null();
  }

private:

  std::unordered_map<utility::hashed_string, state> _state_map;
  std::vector<transition> _transitions;
  parameters _parameters{};

  state _current_state{};
  state _next_state{};
  std::float_t _current_state_time{0.0f};
  std::float_t _next_state_time{0.0f};

  bool _is_in_transition{false};
  std::float_t _cross_fade_duration{0.0f};
  std::float_t _cross_fade_elapsed{0.0f};

}; // class animator 

} // namespace sbx::animations

#endif // LIBSBX_ANIMATIONS_ANIMATOR_HPP_