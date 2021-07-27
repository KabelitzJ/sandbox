#ifndef SBX_CORE_SYSTEM_HPP_
#define SBX_CORE_SYSTEM_HPP_

namespace sbx {

class system {

public:
  system();
  virtual ~system() = default;

  virtual void initialize() = 0;

}; // class system

} // namespace sbx

#endif // SBX_CORE_SYSTEM_HPP_
