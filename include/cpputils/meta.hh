#pragma once

#include <utility>

namespace utils::meta {
template<typename ...Ts>
struct Overload : Ts... {
  using Ts::operator()...;
};

template<typename ...Ts>
Overload(Ts...) -> Overload<Ts...>;

template<typename T>
constexpr inline const char* TypeStr = "unknown type";

#define DECL_TYPESTR(type) \
  template<> \
  constexpr inline const char* TypeStr<type> = #type;

DECL_TYPESTR(bool)
DECL_TYPESTR(char)
DECL_TYPESTR(unsigned char)
DECL_TYPESTR(signed char)
DECL_TYPESTR(short)
DECL_TYPESTR(unsigned short)
DECL_TYPESTR(int)
DECL_TYPESTR(unsigned int)
DECL_TYPESTR(long)
DECL_TYPESTR(unsigned long)
DECL_TYPESTR(long long)
DECL_TYPESTR(unsigned long long)
DECL_TYPESTR(float)
DECL_TYPESTR(double)
DECL_TYPESTR(long double)

#undef DECL_TYPESTR

}  // namespace utils::meta
