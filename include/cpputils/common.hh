#pragma once

#include <sstream>
#include <string_view>
#include <algorithm>

namespace utils {

template <typename T, typename U>
bool OneOf(T val, std::initializer_list<U> list) {
  return std::find(list.begin(), list.end(), val) != list.end();
}

template <typename T, typename Container>
bool OneOf(T val, const Container& c) {
  return std::find(c.begin(), c.end(), val) != c.end();
}

} // namespace utils

using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i32 = std::int32_t;
using i64 = std::int64_t;


