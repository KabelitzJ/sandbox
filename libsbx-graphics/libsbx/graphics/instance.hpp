#ifndef LIBSBX_GRAPHICS_INSTANCE_HPP_
#define LIBSBX_GRAPHICS_INSTANCE_HPP_

namespace sbx::graphics {

class instance {

public:

  instance();

  ~instance();

private:

  void _initialize();

  void _terminate();

}; // class instance

} // namespace sbx::graphics

#endif // LIBSBX_GRAPHICS_INSTANCE_HPP_
