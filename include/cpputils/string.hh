#include <string>
#include <unordered_set>
#include <vector>
#include <type_traits>
#include <sstream>

namespace utils {

namespace detail {
  inline const std::unordered_set WS = {' ', '\f', '\n', '\r', '\t', '\v'};
}  // namespace utils::detail

/*******************************************************************************
*                                    Trim                                     *
*******************************************************************************/

inline std::string Trim(const std::string& s, const std::unordered_set<char>& trimmedChars) {
  const auto isNotTrimmedChar = [&trimmedChars](char c) { return trimmedChars.find(c) != trimmedChars.end(); };
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

inline std::vector<std::string> Split(const std::string& s, const std::unordered_set<char>& sepChars) {
  const auto isNotSepChar = [&sepChars](char c) { return sepChars.find(c) != sepChars.end(); };
  auto begin = std::find_if(s.begin(), s.end(), isNotSepChar);
  std::vector<std::string> result;  // TODO: preallocate vector space
  while (begin != s.end()) {
    auto end = std::find_if_not(begin, s.end(), isNotSepChar);
    result.emplace_back(begin, end);
    begin = std::find_if(end, s.end(), isNotSepChar);
  }
  return result;
}

inline std::vector<std::string> Split(const std::string& s) {
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
