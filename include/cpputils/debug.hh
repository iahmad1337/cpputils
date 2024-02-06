#pragma once

#include <cpputils/common.hh>

#include <exception>

#define EXPECT(cond, message) \
  if (!(cond)) { \
    throw std::runtime_error(::utils::Format(__FILE__ ":% %", __LINE__, message)); \
  }
