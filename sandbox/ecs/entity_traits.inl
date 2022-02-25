namespace sbx {

template<entity Entity>
inline constexpr entity_traits<Entity>::entity_type entity_traits<Entity>::to_underlying(const value_type value) noexcept {
  return static_cast<entity_type>(value);
}

} // namespace sbx