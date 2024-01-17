#pragma once

#include <exception>
#include "common.hh"

#define EXPECT(cond, message) \
  if (!(cond)) { \
    throw std::runtime_error(::utils::Format(__FILE__ ":% %", __LINE__, message)); \
  }