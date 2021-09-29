namespace sbx {

template<typename Derived, typename Delta>
inline void basic_process<Derived, Delta>::abort(const bool immediately = false) {
  if(is_alive()) {
    _current_state = state::aborted;

    if(immediately) {
      tick({});
    }
  }
}

template<typename Derived, typename Delta>
inline void basic_process<Derived, Delta>::tick(const Delta delta) {
  switch (_current_state) {
    case state::uninitialized: {
      _next(std::integral_constant<state, state::uninitialized>{});
      _current_state = state::running;
      break;
    }
    case state::running: {
      _next(std::integral_constant<state, state::running>{}, delta);
      break;
    }
    default: {
      break;
    }
  }

  switch(_current_state) {
    case state::succeeded: {
      _next(std::integral_constant<state, state::succeeded>{});
      break;
    }
    case state::failed: {
      _next(std::integral_constant<state, state::failed>{});
      break;
    }
    case state::aborted: {
      _next(std::integral_constant<state, state::aborted>{});
      _current_state = state::failed;
      break;
    }
    default: {
        break;
    }
  }
}

} // namespace sbx
