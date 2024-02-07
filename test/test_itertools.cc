#include <cpputils/common.hh>
#include <cpputils/string.hh>
#include <cpputils/itertools.hh>
#include <iostream>

#include <charconv>
#include <functional>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define EXPECT_THROW_MATCHES(callable, matcher) \
  try { \
    (callable)(); \
  } catch (const std::exception& e) { \
    EXPECT_THAT(e.what(), matcher); \
  }

TEST(ItertoolsTest, VectorView) {
  std::vector<int> v{1, 2, 3, 4, 5};
  auto range = utils::TRangeView{v};
  EXPECT_THAT(ToVector(range), testing::ElementsAre(1, 2, 3, 4, 5));

  v = {};
  range = utils::TRangeView{v};
  EXPECT_THAT(ToVector(range), testing::IsEmpty());
}

TEST(ItertoolsTest, InitializerListView) {
  auto il = std::initializer_list<int>{1, 2, 3};
  auto range = utils::TRangeView{il};
  EXPECT_THAT(ToVector(range), testing::ElementsAre(1, 2, 3));
}

TEST(ItertoolsTest, MappedRange) {
  const auto v = std::vector{1, 2, 3};
  const auto square = [](auto x) { return x * x; };
  const auto cube = [square] (const auto& x) { return x * square(x); };
  const auto quad = [square] (auto& x) { return square(x) * square(x); };
  EXPECT_THAT(utils::ToVector(utils::Map(v, square)), testing::ElementsAre(1, 4, 9));
  EXPECT_THAT(utils::ToVector(utils::Map(v, cube)), testing::ElementsAre(1, 8, 27));
  EXPECT_THAT(utils::ToVector(utils::Map(v, quad)), testing::ElementsAre(1, 16, 81));
  EXPECT_EQ(utils::ToVector(utils::Map(utils::Map(v, square), square)), utils::ToVector(utils::Map(v, quad)));
}
