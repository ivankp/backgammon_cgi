#ifndef IVANP_CAT_HH
#define IVANP_CAT_HH
#ifndef IVANP_STRING_HH

#include <sstream>

namespace ivanp {

template <typename... T>
inline std::string cat(T&&... x) {
  std::stringstream ss;
  using expander = int[];
  (void)expander{0, ((void)(ss << std::forward<T>(x)), 0)...};
  return ss.str();
}
inline std::string cat() { return { }; }

inline std::string cat(std::string x) { return x; }
inline std::string cat(const char* x) { return x; }

}

#endif
#endif
