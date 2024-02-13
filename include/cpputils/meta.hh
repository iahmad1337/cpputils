#pragma once

#include <utility>
#include <tuple>
#include <array>
#include <functional>
#include <type_traits>

namespace utils::meta {
template<typename ...Ts>
struct Overload : Ts... {
  using Ts::operator()...;
};

template<typename ...Ts>
Overload(Ts...) -> Overload<Ts...>;

template<class... Ts>
struct TFalse : std::false_type {};

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

namespace detail {
template<class TTupleLike, class Fn, size_t... Is>
constexpr auto TransformTupleImpl(TTupleLike&& t, Fn&& fn, std::index_sequence<Is...>) {
  return std::make_tuple(std::invoke(std::forward<Fn>(fn), std::get<Is>(std::forward<TTupleLike>(t)))...);
}

template<class TupleLike, class TInit, class ReduceFn, size_t... Is>
constexpr auto TupleReduceImpl(TupleLike&& t, TInit&& init, ReduceFn&& fn, std::index_sequence<Is...>) {
  using TCommon = std::common_type_t<std::tuple_element_t<Is, std::decay_t<TupleLike>>...>;
  std::array<TCommon, sizeof...(Is)> casted = {static_cast<TCommon>(std::get<Is>(std::forward<TupleLike>(t)))...};
  auto result{std::forward<TInit>(init)};
  for (const auto& v : casted) {
    result = std::invoke(std::forward<ReduceFn>(fn), result, v);
  }
  return result;
}

template<class TupleLikeLhs, class TupleLikeRhs, class BinOp, size_t... Is>
constexpr auto ApplyBinOpImpl(TupleLikeLhs lhs, TupleLikeRhs rhs, BinOp&& op, std::index_sequence<Is...>) {
  return std::make_tuple(std::invoke(std::forward<BinOp>(op), std::get<Is>(lhs), std::get<Is>(rhs))...);
}

}  // namespace utils::detail

template<class TTupleLike, class Fn>
constexpr auto TransformTuple(TTupleLike&& t, Fn&& fn) {
  return detail::TransformTupleImpl(
    t,
    fn,
    std::make_index_sequence<std::tuple_size_v<std::decay_t<TTupleLike>>>()
  );
}

template<class TupleLikeLhs, class TupleLikeRhs, class BinOp>
constexpr auto ApplyBinOp(TupleLikeLhs&& lhs, TupleLikeRhs&& rhs, BinOp&& op) {
  constexpr auto TUPLE_SIZE_LHS = std::tuple_size_v<std::decay_t<TupleLikeLhs>>;
  constexpr auto TUPLE_SIZE_RHS = std::tuple_size_v<std::decay_t<TupleLikeRhs>>;
  static_assert(TUPLE_SIZE_LHS == TUPLE_SIZE_RHS, "Tuples should match");
  return detail::ApplyBinOpImpl(
      std::forward<TupleLikeLhs>(lhs),
      std::forward<TupleLikeRhs>(rhs),
      std::forward<BinOp>(op),
      std::make_index_sequence<TUPLE_SIZE_LHS>()
    );
}

template<class TupleLike, class Init, class ReduceFn>
constexpr auto ReduceTuple(TupleLike&& t, Init init, ReduceFn&& fn) {
  constexpr auto TUPLE_SIZE = std::tuple_size_v<std::decay_t<TupleLike>>;
  static_assert(TUPLE_SIZE > 0, "Can't reduce empty tuple!");
  if constexpr (TUPLE_SIZE == 1) {
    return std::get<0>(std::forward<TupleLike>(t));
  } else {
    return detail::TupleReduceImpl(
      std::forward<TupleLike>(t),
      init,
      std::forward<ReduceFn>(fn),
      std::make_index_sequence<TUPLE_SIZE>()
    );
  }
}

}  // namespace utils::meta
