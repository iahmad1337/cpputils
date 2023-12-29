#include "include/cpputils/common.hh"
#include "include/cpputils/string.hh"
#include <iostream>

#include <charconv>

int main() {
  std::string str = "file";
  std::cout << utils::Trim(str) << std::endl;
  auto &trimmedChars = utils::detail::WS;
  const auto isNotTrimmedChar = [](char c) { return trimmedChars.find(c) != trimmedChars.end(); };
  auto end = std::find_if_not(str.rbegin(), str.rend(), isNotTrimmedChar).base();

  std::cout << utils::Format("`%`, size is %", str.substr(0, end - str.begin()), end - str.begin()) << std::endl;

  std::cout << utils::OneOf("hello", {"hello", "hi", "hey"}) << ' ' << utils::OneOf("hello", {"bye", "have a nice one"}) << std::endl;
  auto cont = {"hello", "my", "name", "is", "ahmad"};
  std::cout << utils::OneOf("name", cont) << ' ' << utils::OneOf("bye", cont) << std::endl;
  std::cout << utils::Join({"hello", "my", "name", "is", "ahmad"}, ", ") << std::endl;
  std::cout << utils::Format(R"(printf("\%d\\n", %))", "a") << std::endl;
}
