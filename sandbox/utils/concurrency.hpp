#ifndef SBX_UTIL_CONCURRENCY_HPP_
#define SBX_UTIL_CONCURRENCY_HPP_

#include <future>
#include <utility>

namespace sbx {

template<typename Result, typename... Functions>
Result await_all(Functions&&... functions) {
  return await_all_get<Result>(std::async(std::launch::async, std::forward<Functions>(functions))...);
};

template<typename Result, typename... Futures>
Result await_all_get(Futures&&... futures) {
  return Result{futures.get()...};
}

} // namespace sbx

#endif // SBX_UTIL_CONCURRENCY_HPP_
