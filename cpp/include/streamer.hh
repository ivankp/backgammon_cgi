#ifndef IVANP_STREAMER_HH
#define IVANP_STREAMER_HH

template <typename T>
struct streamer {
  const T& ref;
};
template <typename T> streamer(T) -> streamer<T>;

template <typename T>
std::ostream& operator<<(std::ostream& os, streamer<T> s) {
  os << s.ref;
  return os;
}

template <typename... Ts>
std::ostream& operator<<(std::ostream& os, streamer<std::variant<Ts...>> sv) {
  std::visit([&os](const auto& v) { os << streamer{v}; }, sv.ref);
  return os;
}

#endif
