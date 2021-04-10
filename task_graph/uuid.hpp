#ifndef TASK_GRAPH_uuid_HPP_
#define TASK_GRAPH_uuid_HPP_

#include <chrono>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>

namespace tg {

class uuid {

public:
  using value_type      = std::uint8_t;
  using reference       = std::uint8_t&;
  using const_reference = const std::uint8_t&;
  using iterator        = std::uint8_t*;
  using const_iterator  = const std::uint8_t*;
  using size_type       = std::size_t;
  using difference_type = std::ptrdiff_t;

  uuid();
  uuid(const uuid&) = default;
  uuid(uuid&&) = default;
  ~uuid() = default;

  uuid& operator=(const uuid&) = default;
  uuid& operator=(uuid&&) = default;

  static size_type size();

  iterator begin(); 
  const_iterator begin() const; 
  iterator end(); 
  const_iterator end() const; 

  bool is_nil() const;
  void swap(uuid& rhs);
  std::size_t hash_value() const;

  bool operator==(const uuid& rhs) const;
  bool operator!=(const uuid& rhs) const;
  bool operator<(const uuid& rhs) const;
  bool operator>(const uuid& rhs) const;
  bool operator>=(const uuid& rhs) const;
  bool operator<=(const uuid& rhs) const; 

  std::string to_string() const;

private:
  static char _to_char(std::size_t value);

  static constexpr std::uint32_t _SIZE = 16;

  std::uint8_t _data[_SIZE];

}; // class uuid

inline uuid::uuid() {
  using random_uint_t = unsigned long;

  static thread_local std::random_device rd;
  static thread_local std::mt19937 engine {rd()};

  std::uniform_int_distribution<random_uint_t> distribution(
    std::numeric_limits<random_uint_t>::min(),
    std::numeric_limits<random_uint_t>::max()
  );
  
  int i = 0;
  random_uint_t random_value = distribution(engine);
  for (auto it = begin(); it != end(); ++it, ++i) {
    if (i == sizeof(random_uint_t)) {
      random_value = distribution(engine);
      i = 0;
    }
    *it = static_cast<value_type>((random_value >> (i * 8)) & 0xFF);
  }
  
  // set variant: must be 0b10xxxxxx
  *(begin()+8) &= 0xBF;
  *(begin()+8) |= 0x80;

  // set version: must be 0b0100xxxx
  *(begin()+6) &= 0x4F; //0b01001111
  *(begin()+6) |= 0x40; //0b01000000 
}

inline uuid::size_type uuid::size() {
  return _SIZE;
}

inline uuid::iterator uuid::begin() {
  return _data;
}

inline uuid::const_iterator uuid::begin() const {
  return _data;
}

inline uuid::iterator uuid::end() {
  return _data + _SIZE;
}

inline uuid::const_iterator uuid::end() const {
  return _data + _SIZE;
}

inline bool uuid::is_nil() const {
  for (std::size_t i = 0; i < sizeof(_data); ++i) {
    if (_data[i] != 0u) {
      return false;
    }
  }
  return true;
}

inline void uuid::swap(uuid& rhs) {
  uuid tmp = *this;
  *this = rhs;
  rhs = tmp;
}

inline std::size_t uuid::hash_value() const {
  std::size_t seed = 0;
  for (const_iterator i = begin(); i != end(); ++i) {
    seed ^= static_cast<std::size_t>(*i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
  return seed;
}

inline bool uuid::operator==(const uuid& rhs) const {
  return std::memcmp(_data, rhs._data, sizeof(_data)) == 0;
}

inline bool uuid::operator!=(const uuid& rhs) const {
  return std::memcmp(_data, rhs._data, sizeof(_data)) != 0;
}

inline bool uuid::operator<(const uuid& rhs) const {
  return std::memcmp(_data, rhs._data, sizeof(_data)) < 0;
}

inline bool uuid::operator>(const uuid& rhs) const {
  return std::memcmp(_data, rhs._data, sizeof(_data)) > 0;
}

inline bool uuid::operator>=(const uuid& rhs) const {
  return std::memcmp(_data, rhs._data, sizeof(_data)) >= 0;
}

inline bool uuid::operator<=(const uuid& rhs) const {
  return std::memcmp(_data, rhs._data, sizeof(_data)) <= 0;
}

inline std::string uuid::to_string() const {
  std::string result;
  result.reserve(36);

  std::size_t i=0;
  for (const_iterator it = begin(); it!=end(); ++it, ++i) {

    const size_t hi = ((*it) >> 4) & 0x0F;
    result += _to_char(hi);

    const size_t lo = (*it) & 0x0F;
    result += _to_char(lo);

    if (i == 3 || i == 5 || i == 7 || i == 9) {
      result += '-';
    }
  }

  return result;
}

inline char uuid::_to_char(std::size_t value) {
  if (value <= 9) {
    return static_cast<char>('0' + value);
  }

  return static_cast<char>('a' + (value - 10));
}

inline void swap(uuid& lhs, uuid& rhs) {
  lhs.swap(rhs);
}

inline std::ostream& operator << (std::ostream& os, const uuid& rhs) {
  return os << rhs.to_string();
}

} // namespace tg

namespace std {

template<>
struct hash<tg::uuid> {
  size_t operator()(const tg::uuid& rhs) const { return rhs.hash_value(); }
};


}

#endif // TASK_GRAPH_uuid_HPP_