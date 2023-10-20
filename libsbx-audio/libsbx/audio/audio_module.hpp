#ifndef LIBSBX_AUDIO_AUDIO_MODULE_HPP_
#define LIBSBX_AUDIO_AUDIO_MODULE_HPP_

#include <libsbx/core/module.hpp>

#include <libsbx/assets/assets_module.hpp>

namespace sbx::audio {

class audio_module : public core::module<audio_module> {

  inline static const auto is_registered = register_module(stage::normal, dependencies<assets::assets_module>{});

public:

  audio_module();

  ~audio_module() override;

  auto update() -> void override;

private:



}; // class audio_module

} // namespace sbx::audio

#endif // LIBSBX_AUDIO_AUDIO_MODULE_HPP_
