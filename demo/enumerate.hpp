#ifndef DEMO_ENUMERATE_HPP_
#define DEMO_ENUMERATE_HPP_

#include <algorithm>
#include <vector>

namespace demo {

namespace detail {

template<typename Type>
struct entry {

  using value_type = Type;
  using index_type = std::size_t;

  value_type value{};
  index_type index{}; 

}; // struct entry

} // namespace detail

template<typename container>
std::vector<detail::entry<typename container::value_type>> enumerate(const container& enumerable) {
  // Very crude implementation - until std::views::enumerate() is available
  auto result = std::vector<detail::entry<typename container::value_type>>{};
  std::size_t index = 0;

  std::for_each(std::begin(enumerable), std::end(enumerable), [&](const auto& value) {
    result.emplace_back(detail::entry{value, index});
    ++index;
  });

  return result;
}

} // namespace dem

#endif // DEMO_ENUMERATE_HPP_
