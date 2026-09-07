// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Jonas Kabelitz
#ifndef LIBSBX_ASSETS_LOADABLE_HPP_
#define LIBSBX_ASSETS_LOADABLE_HPP_

#include <cstdint>

namespace sbx::assets {

/**
 * @brief Every asset type's content-readiness state, reachable straight off an asset_handle<T> the
 * same way is_valid() already is (see asset_handle.hpp's is_loaded()/generation()).
 *
 * `generation()` starts at 0 (never loaded) and is bumped once each time real content replaces a
 * placeholder -- or, for material/particle_effect/animation_graph, each time an update_* call
 * applies a live edit, since that's a real content change too. `is_loaded()` is just `generation()
 * > 0`. Lives on the asset object itself (the thing an asset_handle<T> actually points at), not on
 * the handle -- multiple handle copies share one object and need to see the same state, which a
 * counter on the handle wrapper itself couldn't give them.
 */
class loadable {

public:

  [[nodiscard]] auto generation() const noexcept -> std::uint64_t {
    return _generation;
  }

  [[nodiscard]] auto is_loaded() const noexcept -> bool {
    return _generation > 0u;
  }

protected:

  auto _bump_generation() noexcept -> void {
    ++_generation;
  }

private:

  std::uint64_t _generation{0u};

}; // class loadable

} // namespace sbx::assets

#endif // LIBSBX_ASSETS_LOADABLE_HPP_
