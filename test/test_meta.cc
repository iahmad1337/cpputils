#include <cpputils/common.hh>
#include <cpputils/string.hh>
#include <cpputils/meta.hh>
#include <iostream>

#include <charconv>
#include <functional>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(MetaTest, TransformTuple) {
  const auto v = std::vector{1, 2, 3};
  const auto square = [](auto x) { return x * x; };
  const auto cube = [square] (const auto& x) { return x * square(x); };
  const auto quad = [square] (auto& x) { return square(x) * square(x); };
  EXPECT_EQ(utils::meta::TransformTuple(std::tuple{1, 2, 3}, square), (std::tuple{1, 4, 9}));
  EXPECT_EQ(utils::meta::TransformTuple(std::tuple{1, 2, 3}, cube), (std::tuple{1, 8, 27}));
  EXPECT_EQ(utils::meta::TransformTuple(std::tuple{1, 2, 3}, quad), (std::tuple{1, 16, 81}));
}
