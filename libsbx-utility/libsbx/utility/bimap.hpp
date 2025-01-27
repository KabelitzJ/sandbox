#ifndef LIBSBX_UTILITY_BIMAP_HPP_
#define LIBSBX_UTILITY_BIMAP_HPP_

#include <unordered_map>
#include <optional>

namespace sbx::utility {

template<typename Key, typename Value>
class bimap {

  using forward_map_type = std::unordered_map<Key, Value>;
  using reverse_map_type = std::unordered_map<Value, Key>;

public:

  using key_type = Key;
  using value_type = Value;

  auto insert(const Key& key, const Value& value) -> void {
    _forward.insert({key, value});
    _reverse.insert({value, key});
  }

  auto erase_key(const key_type& key) -> void {
    _reverse.erase(_forward[key]);
    _forward.erase(key);
  }

  auto erase_value(const value_type& value) -> void {
    _forward.erase(_reverse[value]);
    _reverse.erase(value);
  }

  auto find_key(const value_type& value) -> std::optional<key_type> {
    if (auto entry = _reverse.find(value); entry != _reverse.end()) {
      return entry->second;
    }

    return std::nullopt;
  }

  auto find_value(const key_type& key) -> std::optional<value_type> {
    if (auto entry = _forward.find(key); entry != _forward.end()) {
      return entry->second;
    }

    return std::nullopt;
  }

private:

  std::unordered_map<key_type, value_type> _forward;
  std::unordered_map<value_type, key_type> _reverse;

}; // class bimap

} // namespace sbx::utility

#endif // LIBSBX_UTILITY_BIMAP_HPP_
