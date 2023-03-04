#pragma once

#include <sstream>
#include <string_view>
#include <algorithm>

namespace utils {

template <typename T>
std::string ToString(const T& x) {
  std::ostringstream os;
  os << x;
  return os.str();
}

template <typename... Args>
std::string Format(std::string_view formatStr, Args&&... args) {
  if constexpr (sizeof...(Args) == 0) {
    return std::string{formatStr};
  }
  if (std::count(formatStr.begin(), formatStr.end(), '%') != sizeof...(Args)) {
    throw std::runtime_error(
        "Number of arguments doesn't match number of replacement spots");
  }
  auto replacements = {ToString(std::forward<Args>(args))...};
  auto currentReplacement = replacements.begin();
  std::ostringstream result;
  for (auto c : formatStr) {
    if (c == '%') {
      result << *currentReplacement;
      currentReplacement++;
    } else {
      result << c;
    }
  }
  return result.str();
}

class MakeString {
public:
  template<class T>
  MakeString& operator<< (const T& arg) {
    ss << arg;
    return *this;
  }
  operator std::string() const {
    return ss.str();
  }
private:
  std::stringstream ss;
};

} // namespace utils
