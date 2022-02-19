namespace sbx {

template<entity Entity>
inline constexpr typename entity_traits<Entity>::entity_type entity_traits<Entity>::to_integral(const value_type value) noexcept {
  return static_cast<entity_type>(value);
}

template<entity Entity>
inline constexpr typename entity_traits<Entity>::entity_type entity_traits<Entity>::to_entity(const value_type value) noexcept {
  return (to_integral(value) & entity_mask);
}

template<entity Entity>
inline constexpr typename entity_traits<Entity>::version_type entity_traits<Entity>::to_version(const value_type value) noexcept {
  return (to_integral(value) >> entity_shift);
}

template<entity Entity>
inline constexpr typename entity_traits<Entity>::value_type entity_traits<Entity>::construct(const entity_type entity, const version_type version) noexcept {
  return value_type{(entity & entity_mask) | (static_cast<entity_type>(version) << entity_shift)};
}

template<entity Entity>
inline constexpr typename entity_traits<Entity>::value_type entity_traits<Entity>::combine(const entity_type lhs, const entity_type rhs) noexcept {
  return value_type{(lhs & entity_mask) | (rhs & (version_mask << entity_shift))};
}

template<entity Entity>
inline constexpr null_entity_t::operator Entity() const noexcept {
  using entity_traits = entity_traits<Entity>;
  return entity_traits::combine(entity_traits::reserved, entity_traits::reserved);
}

inline constexpr bool operator==([[maybe_unused]] const null_entity_t lhs, [[maybe_unused]] const null_entity_t rhs) noexcept {
  return true;
}

template<entity Entity>
inline constexpr bool operator==(const Entity lhs, const null_entity_t rhs) noexcept {
  using entity_traits = entity_traits<Entity>;
  return (entity_traits::to_entity(lhs) == entity_traits::to_entity(rhs));
}

template<entity Entity>
inline constexpr placeholder_entity_t::operator Entity() const noexcept {
  using entity_traits = entity_traits<Entity>;
  return entity_traits::combine(entity_traits::reserved, entity_traits::reserved);
}

inline constexpr bool operator==([[maybe_unused]] const placeholder_entity_t lhs, [[maybe_unused]] const placeholder_entity_t rhs) noexcept {
  return true;
}

template<entity Entity>
inline constexpr bool operator==(const Entity lhs, const placeholder_entity_t rhs) noexcept {
  using entity_traits = entity_traits<Entity>;
  return (entity_traits::to_version(lhs) == entity_traits::to_version(rhs));
}

} // namespace sbx