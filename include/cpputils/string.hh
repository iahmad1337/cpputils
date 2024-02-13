#pragma once

#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <type_traits>
#include <sstream>
#include <algorithm>

namespace utils {

namespace detail {
  inline const std::unordered_set WS = {' ', '\f', '\n', '\r', '\t', '\v'};
}  // namespace utils::detail

template <typename T>
std::string ToString(const T& x) {
  std::ostringstream os;
  os << x;
  return os.str();
}

template <typename... Args>
std::string Format(std::string_view formatStr, Args&&... args) {
  constexpr static auto REPORT_MISMATCH = [] () {
    throw std::runtime_error(
        "Number of arguments doesn't match number of replacement spots");
  };
  if constexpr (sizeof...(Args) == 0) {
    return std::string{formatStr};
  }
  std::initializer_list<std::string> replacements = {ToString(std::forward<Args>(args))...};
  auto currentReplacement = replacements.begin();
  std::ostringstream result;
  bool skip{false};
  for (auto c : formatStr) {
    if (skip) {
      skip = false;
      result << c;
      continue;
    }
    if (c == '%') {
      if (currentReplacement == replacements.end()) {
        REPORT_MISMATCH();
      }
      result << *currentReplacement;
      currentReplacement++;
    } else if (c == '\\') {
      skip = true;
    } else {
      result << c;
    }
  }
  if (currentReplacement != replacements.end()) {
    REPORT_MISMATCH();
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

/*******************************************************************************
*                                    Trim                                     *
*******************************************************************************/

inline std::string Trim(const std::string& s, const std::unordered_set<char>& trimmedChars) {
  const auto isNotTrimmedChar = [&trimmedChars](char c) { return trimmedChars.find(c) == trimmedChars.end(); };
  const auto begin = std::find_if(s.begin(), s.end(), isNotTrimmedChar);
  const auto end = std::find_if(s.rbegin(), s.rend(), isNotTrimmedChar).base();
  std::string result;
  if (begin < end) {
    result.resize(end - begin);
    std::copy(begin, end, result.begin());
  }
  return result;
}

inline std::string Trim(const std::string& s) {
  return Trim(s, detail::WS);
}

/*******************************************************************************
*                                    Split                                    *
*******************************************************************************/

inline std::vector<std::string> Split(const std::string_view& s, const std::unordered_set<char>& sepChars) {
  const auto isSepChar = [&sepChars](char c) { return sepChars.find(c) != sepChars.end(); };
  auto begin = std::find_if_not(s.begin(), s.end(), isSepChar);
  std::vector<std::string> result;  // TODO: preallocate vector space
  while (begin != s.end()) {
    auto end = std::find_if(begin, s.end(), isSepChar);
    result.emplace_back(begin, end);
    begin = std::find_if_not(end, s.end(), isSepChar);
  }
  return result;
}

inline std::vector<std::string> Split(const std::string_view& s) {
  return Split(s, detail::WS);
}

/*******************************************************************************
*                                    Join                                     *
*******************************************************************************/

template<typename ItBegin, typename ItEnd>
inline std::string Join(std::string_view sep, ItBegin begin, ItEnd end) {
  // TODO: restrictions on types
  std::string result;
  if (begin != end) {
    result = *begin;
    begin++;
    while (begin != end) {
      result.append(sep);
      result.append(*begin);
      begin++;
    }
  }
  return result;
}

template<typename Container>
inline std::string Join(const Container& cont, std::string_view sep = ", ") {
  return Join(sep, cont.begin(), cont.end());
}

template<typename T>
inline std::string Join(std::initializer_list<T> l, std::string_view sep = ", ") {
  return Join(sep, l.begin(), l.end());
}

/*******************************************************************************
*                                   Replace                                   *
*******************************************************************************/

inline std::string Replace(std::string_view text, std::vector<std::pair<std::string_view, std::string_view>> replacementPairs) {
  std::string result; // TODO: reserve space for result
  for (std::size_t i = 0; i < text.size();) {
    bool replaced = false;
    for (const auto& [pattern, replacement] : replacementPairs) {
      if (text.size() - i < pattern.size()) {
        continue;
      }
      if (text.compare(i, pattern.size(), pattern) == 0) {
        result.append(replacement);
        i += pattern.size();
        replaced = true;
        break;
      }
    }
    if (!replaced) {
      result.push_back(text[i]);
      i++;
    }
  }
  return result;
}

}  // namespace utils
