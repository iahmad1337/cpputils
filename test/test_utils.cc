#include <cpputils/common.hh>
#include <cpputils/string.hh>
#include <iostream>

#include <charconv>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define EXPECT_THROW_MATCHES(callable, matcher) \
  try { \
    (callable)(); \
  } catch (const std::exception& e) { \
    EXPECT_THAT(e.what(), matcher); \
  }

TEST(FormatTest, FormatValid) {
  EXPECT_EQ(utils::Format("%", 42), "42");
  EXPECT_EQ(utils::Format("\%"), "%");
  EXPECT_EQ(utils::Format(""), "");
  EXPECT_EQ(utils::Format("%%", 'a', 'b'), "ab");
}

TEST(FormatTest, InvalidArgumentNumber) {
  const auto ERROR_MESSAGE_MATCHER = testing::HasSubstr("Number of arguments doesn't match number of replacement spots");
  EXPECT_THROW_MATCHES([]{ utils::Format("\%", 42); }, ERROR_MESSAGE_MATCHER);
  EXPECT_THROW_MATCHES([]{ utils::Format("%"); }, ERROR_MESSAGE_MATCHER);
  EXPECT_THROW_MATCHES([]{ utils::Format("%%", 42); }, ERROR_MESSAGE_MATCHER);
}

// int main() {
//   std::string str = "file";
//   std::cout << utils::Trim(str) << std::endl;
//   auto &trimmedChars = utils::detail::WS;
//   const auto isNotTrimmedChar = [](char c) { return trimmedChars.find(c) != trimmedChars.end(); };
//   auto end = std::find_if_not(str.rbegin(), str.rend(), isNotTrimmedChar).base();
//
//   std::cout << utils::Format("`%`, size is %", str.substr(0, end - str.begin()), end - str.begin()) << std::endl;
//
//   std::cout << utils::OneOf("hello", {"hello", "hi", "hey"}) << ' ' << utils::OneOf("hello", {"bye", "have a nice one"}) << std::endl;
//   auto cont = {"hello", "my", "name", "is", "ahmad"};
//   std::cout << utils::OneOf("name", cont) << ' ' << utils::OneOf("bye", cont) << std::endl;
//   std::cout << utils::Join({"hello", "my", "name", "is", "ahmad"}, ", ") << std::endl;
//   std::cout << utils::Format(R"(printf("\%d\\n", %))", "a") << std::endl;
// }
