#pragma once

#include <cpputils/string.hh>

#include <tuple>
#include <string_view>
#include <vector>
#include <type_traits>
#include <utility>

namespace utils::detail {
template<typename T>
constexpr std::string_view TypeName() {
  using namespace std::literals;
  constexpr std::string_view PRETTY = __PRETTY_FUNCTION__;

  // Based on godbolt output for trunk gcc and clang (02.13.2024)
#if defined(__GNUC__) || defined(__GNUG__)
  auto start = "with T = "sv;
  auto end = "; "sv;
#elif defined(__clang__)
  auto start = "[T = "_sv;
  auto end = "]"_sv;
#endif
  return PRETTY.substr(
      PRETTY.find(start) + start.size(),
      PRETTY.find(end) - (PRETTY.find(start) + start.size())
  );
}

struct TTypeDescriptor;
struct TFieldDescriptor;
struct TStructDescriptor;

struct TTypeDescriptor {

  template<class T>
  static const TTypeDescriptor* Get() {
    static constexpr TTypeDescriptor descriptor = {
      TypeName<T>()
    };
    return &descriptor;
  }

  // Some other characteristics may come in handy later on
  std::string_view Name;
};

template<class... Ts>
struct TypeList {
  constexpr TypeList(Ts...) {}
};

template<class... Ts>
TypeList(Ts...) -> TypeList<std::decay_t<Ts>...>;

struct TStructDescriptor {

  template<class TStruct, class... TFields>
  static const TStructDescriptor* Get(
      TStruct,
      std::string_view fieldNames,
      TypeList<TFields...>
  ) {
    static const TStructDescriptor descriptor = {
      TypeName<TStruct>(),
      Split(fieldNames, {' ', ','}),
      std::vector{
        TTypeDescriptor::Get<TFields>()...
      }
    };
    return &descriptor;
  }

  std::string_view Name;
  std::vector<std::string> FieldNames;
  std::vector<const TTypeDescriptor*> FieldDescriptors;
};

}  // namespace utils::detail

#define REFLECT(STRUCT, ...) \
  auto GetDescriptorHelper() const noexcept { \
    return utils::detail::TStructDescriptor::Get( \
      STRUCT{}, \
      #__VA_ARGS__, \
      ::utils::detail::TypeList(__VA_ARGS__) \
    ); \
  } \
  auto ToTuple() const { \
    return std::make_tuple(__VA_ARGS__); \
  } \
  static const utils::detail::TStructDescriptor* GetDescriptor() { \
    return STRUCT{}.GetDescriptorHelper(); \
  }

