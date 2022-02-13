namespace sbx {

template<std::unsigned_integral Type>
inline constexpr entity<Type>::entity(const Type id) noexcept
: _id{id} { }

template<std::unsigned_integral Type>
inline constexpr entity<Type>::entity(entity<Type>&& other) noexcept
: _id{other.id} { }

template<std::unsigned_integral Type>
inline entity<Type>::~entity() noexcept {

}

template<std::unsigned_integral Type>
inline constexpr entity<Type>& entity<Type>::operator=(entity<Type>&& other) noexcept {
  _id = other._id;

  return *this;
}

template<std::unsigned_integral Type>
inline constexpr bool operator==(const entity<Type>& lhs, const entity<Type>& rhs) noexcept {
  return lhs._id == rhs._id;
}

} // namespace sbx