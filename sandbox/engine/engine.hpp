#ifndef SBX_ENGINE_HPP_
#define SBX_ENGINE_HPP_

#include <ui/window.hpp>

namespace sbx {

class engine {

public:

  engine();

  engine(const engine&) = delete;

  engine(engine&&) = delete;

  ~engine();

  engine& operator=(const engine&) = delete;

  engine& operator=(engine&&) = delete;

  void start();

private:

  window _window{};

}; // class engine

} // namespace sbx

#endif // SBX_ENGINE_HPP_
