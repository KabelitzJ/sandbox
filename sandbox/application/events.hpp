#ifndef SBX_APPLICATION_EVENTS_HPP_
#define SBX_APPLICATION_EVENTS_HPP_

#include <types/primitives.hpp>

namespace sbx {

struct window_closed_event { };

struct key_event {

  key_event(int32 _key, int32 _scancode, int32 _action, int32 _mods)
  : key{_key},
    scancode{_scancode},
    action{_action},
    mods{_mods} { }

  int32 key;
  int32 scancode;
  int32 action;
  int32 mods;

};

} // namespace sbx

#endif // SBX_APPLICATION_EVENTS_HPP_
