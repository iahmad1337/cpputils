#include <cpputils/reflect.hh>

#include <iostream>
#include <string_view>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace TypeNameTest {
  using namespace std::literals;
  static_assert(utils::detail::TypeName<int>() == "int"sv);
  static_assert(utils::detail::TypeName<std::ios_base>() == "std::ios_base"sv);
  static_assert(utils::detail::TypeName<std::vector<int>>() == "std::vector<int>"sv);
}

struct TMyType {
  int f1;
  float f2;
  char f3;

  REFLECT(TMyType, f1, f2, f3);
};

TEST(ReflectTest, TransformTuple) {
  ASSERT_EQ(TMyType::GetDescriptor()->Name, "TMyType");
  ASSERT_THAT(TMyType::GetDescriptor()->FieldNames, testing::ElementsAre("f1", "f2", "f3"));
  ASSERT_THAT((TMyType{1, 2.5, 'i'}.ToTuple()), testing::FieldsAre(1, 2.5, 'i'));
}

