#include "vector4.hpp"

namespace sbx {

vector4::vector4() 
: _x{0.0f},
  _y{0.0f},
  _z{0.0f},
  _w{0.0f} { }

vector4::vector4(const float32 x, const float32 y, const float32 z, const float32 w)
: _x{x},
  _y{y},
  _z{z},
  _w{w} { }

} // namespace sbx
