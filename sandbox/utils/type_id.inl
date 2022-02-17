namespace sbx {

template<std::unsigned_integral Type>
inline Type id_sequence<Type>::next() noexcept {
  static auto id = Type{0};
  return id++;
}

template<typename Type, typename Id, id_sequence_type Sequence>
inline Id type_id<Type, Id, Sequence>::value() noexcept {
  static const auto id = Sequence::next();
  return id;
}

template<typename Type, typename Id, id_sequence_type Sequence>
inline constexpr type_id<Type, Id, Sequence>::operator id_type() const noexcept {
  return value();
}

} // namespace sbx