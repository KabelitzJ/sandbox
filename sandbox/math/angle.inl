namespace sbx {

template<typename Type>
requires std::floating_point<Type>
constexpr bool operator==(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) == static_cast<Type>(rhs);
}

template<typename Type>
requires std::floating_point<Type>  
constexpr std::strong_ordering operator<=>(const degrees<Type>& lhs, const degrees<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) <=> static_cast<Type>(rhs);
}

template<typename Type>
requires std::floating_point<Type>
constexpr bool operator==(const radians<Type>& lhs, const radians<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) == static_cast<Type>(rhs);
}

template<typename Type>
requires std::floating_point<Type>  
constexpr std::strong_ordering operator<=>(const radians<Type>& lhs, const radians<Type>& rhs) noexcept {
  return static_cast<Type>(lhs) <=> static_cast<Type>(rhs);
}

template<typename Type>
requires std::floating_point<Type>
constexpr bool operator==(const angle<Type>& lhs, const angle<Type>& rhs) noexcept {
  return lhs.to_degrees() == rhs.to_degrees();
}

template<typename Type>
requires std::floating_point<Type>  
constexpr std::strong_ordering operator<=>(const angle<Type>& lhs, const angle<Type>& rhs) noexcept {
  return lhs.to_degrees() <=> rhs.to_degrees();
}

} // namespace sbx