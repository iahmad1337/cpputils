#include <cpputils/common.hh>
#include <cpputils/string.hh>
#include <cpputils/itertools.hh>
#include <iostream>

#include <charconv>
#include <functional>
#include <type_traits>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define EXPECT_THROW_MATCHES(callable, matcher) \
  try { \
    (callable)(); \
  } catch (const std::exception& e) { \
    EXPECT_THAT(e.what(), matcher); \
  }

std::vector<int> empty;

TEST(RangeViewTest, VectorView) {
  std::vector<int> v{1, 2, 3, 4, 5};
  auto range = utils::MakeView(v);
  EXPECT_THAT(utils::ToVector(range), testing::ElementsAre(1, 2, 3, 4, 5));

  v = {};
  range = utils::MakeView(v);
  EXPECT_THAT(utils::ToVector(range), testing::IsEmpty());
}

TEST(RangeViewTest, InitializerListView) {
  auto il = std::initializer_list<int>{1, 2, 3};
  auto range = utils::MakeView(il);
  EXPECT_THAT(utils::ToVector(range), testing::ElementsAre(1, 2, 3));
}

TEST(MappedRangeTest, MappedRange) {
  const auto v = std::vector{1, 2, 3};
  const auto square = [](auto x) { return x * x; };
  const auto cube = [square] (const auto& x) { return x * square(x); };
  const auto quad = [square] (auto& x) { return square(x) * square(x); };
  EXPECT_THAT(utils::Map(v, square), testing::ElementsAre(1, 4, 9));
  EXPECT_THAT(utils::Map(v, cube), testing::ElementsAre(1, 8, 27));
  EXPECT_THAT(utils::Map(v, quad), testing::ElementsAre(1, 16, 81));
  EXPECT_EQ(utils::ToVector(utils::Map(utils::Map(v, square), square)), utils::ToVector(utils::Map(v, quad)));

  EXPECT_THAT(utils::Map(empty, square), testing::ElementsAre());
}

TEST(ZippedRangeTest, TripleVector) {
  const auto v1 = std::vector{1, 2, 3};
  const auto v2 = std::vector{4, 5, 6};
  const auto v3 = std::vector{'a', 'b', 'c'};
  const auto zipped = utils::Zip(v1, v2, v3);
  EXPECT_THAT(zipped, testing::ElementsAre(
    testing::FieldsAre(1, 4, 'a'),
    testing::FieldsAre(2, 5, 'b'),
    testing::FieldsAre(3, 6, 'c')
  ));
}

TEST(ZippedRangeTest, RangeBasedFor) {
  const auto v1 = std::vector{1, 2, 3};
  const auto v3 = std::vector{'a', 'b', 'c'};
  std::vector<std::tuple<int, char>> result;
  for (auto [x, y] : utils::Zip(v1, v3)) {
    result.emplace_back(x, y);
  }
  EXPECT_THAT(result, testing::ElementsAre(
    testing::FieldsAre(1, 'a'),
    testing::FieldsAre(2, 'b'),
    testing::FieldsAre(3, 'c')
  ));
}

TEST(ZipMapTest, ZippedMap) {
  const auto v1 = std::vector{1, 2, 3};
  const auto v3 = std::vector{'a', 'b', 'c'};
  const auto result = utils::Zip(
    v1,
    utils::Map(v3, [](char x) -> int { return x - 'a'; })
  );
  EXPECT_THAT(result, testing::ElementsAre(
    testing::FieldsAre(1, 0),
    testing::FieldsAre(2, 1),
    testing::FieldsAre(3, 2)
  ));
}

TEST(ZipMapTest, NestedMap) {
  const auto v1 = std::vector{1, 2, 3};
  const auto v3 = std::vector{'a', 'b', 'c'};
  const auto result = utils::Map(
    utils::Zip(
      v1,
      utils::Map(v3, [](char x) -> int { return x - 'a'; })
    ),
    [] (const auto& x) { return utils::meta::ReduceTuple(x, 0, std::plus{}); }
  );
  EXPECT_THAT(result, testing::ElementsAre(1, 3, 5));
}


TEST(FilterTest, SimpleFilter) {
  const auto v = std::vector{-5, -4, -3, -2, -1, 0, 1, 2, 3, 4, 5};
  const auto odd = [](auto x) { return x % 2 != 0; };
  const auto even = [](auto x) { return x % 2 == 0; };
  const auto positive = [] (const auto& x) { return x > 0; };
  const auto zero = [] (const auto& x) { return x == 0; };
  const auto negative = [] (const auto& x) { return x < 0; };
  const auto always_false = [] (const auto&) { return false; };
  EXPECT_THAT(utils::Filter(v, odd), testing::ElementsAre(-5, -3, -1, 1, 3, 5));
  EXPECT_THAT(utils::Filter(v, even), testing::ElementsAre(-4, -2, 0, 2, 4));
  EXPECT_THAT(utils::Filter(v, positive), testing::ElementsAre(1, 2, 3, 4, 5));
  EXPECT_THAT(utils::Filter(v, zero), testing::ElementsAre(0));
  EXPECT_THAT(utils::Filter(v, negative), testing::ElementsAre(-5, -4, -3, -2, -1));
  EXPECT_THAT(utils::Filter(v, always_false), testing::ElementsAre());

  EXPECT_THAT(utils::Filter(empty, always_false), testing::ElementsAre());
}
