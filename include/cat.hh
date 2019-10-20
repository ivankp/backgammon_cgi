#ifndef IVANP_CAT_HH
#define IVANP_CAT_HH
#ifndef IVANP_STRING_HH

namespace ivanp {

template <typename... T>
std::string cat(T&&... x) {
  std::stringstream ss;
  using expander = int[];
  (void)expander{0, ((void)(ss << std::forward<T>(x)), 0)...};
  return ss.str();
}
std::string cat() { return { }; }

std::string cat(std::string x) { return x; }
std::string cat(const char* x) { return x; }

}

#endif
#endif
