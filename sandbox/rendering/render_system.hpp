#ifndef SBX_RENDERING_RENDER_SYSTEM_HPP_
#define SBX_RENDERING_RENDER_SYSTEM_HPP_

#include <core/system.hpp>
#include <types/transform.hpp>

#include "mesh.hpp"
#include "render_batch.hpp"

namespace sbx {

class render_system final : public system {

public:

  render_system();
  ~render_system() = default;

  void initialize() override;
  void update(const time delta_time) override;
  void terminate() override;

private:

  void _add_to_batch(const mesh& mesh, const transform& transform);
  void _flush_batch();
  void _reset_batch();

  render_batch _batch{};

}; // class render_system

} // namespace sbx

#endif // SBX_RENDERING_RENDER_SYSTEM_HPP_
