#ifndef LIBSBX_CORE_PROFILER_HPP_
#define LIBSBX_CORE_PROFILER_HPP_

#include <string>
#include <string_view>
#include <unordered_map>
#include <ranges>

#include <libsbx/units/time.hpp>

#include <libsbx/utility/hashed_string.hpp>

namespace sbx::core {

class profiler {

public:

  struct group_entry {
    std::string name;
    units::second measurement;
  }; // struct group_entry

  struct group {
    std::vector<group_entry> entries;
    units::second overall;
  }; // struct group

  auto submit(const utility::hashed_string& scope, const units::second& measurement) -> void {
    _measurements[scope] = measurement;
  }

  template<typename Callable>
  requires (std::is_invocable_v<Callable, const utility::hashed_string&, const group&>)
  auto for_each(Callable&& callable) -> void {
    auto grouped = std::unordered_map<utility::hashed_string, group>{};

    for (const auto& [scope, measurement] : _measurements) {
      const auto position = scope.rfind("::");

      const auto has_namespace = (position != utility::hashed_string::npos);

      const auto group = has_namespace ? scope.substr(0, position) : scope;
      const auto name = has_namespace ? scope.substr(position + 2u) : scope;

      if (!has_namespace) {
        grouped[group].overall = measurement;
      } else {
        grouped[group].entries.emplace_back(name.str(), measurement);
      }
    }

    for (auto& [group_name, group] : grouped) {
      std::sort(group.entries.begin(), group.entries.end(), [](const auto& lhs, const auto& rhs){
        return lhs.measurement.value() > rhs.measurement.value();
      });

      std::invoke(callable, group_name, group);
    }
  }

private:

  std::unordered_map<utility::hashed_string, units::second> _measurements;

}; // class profiler

} // namespace sbx::core

#endif // LIBSBX_CORE_PROFILER_HPP_
