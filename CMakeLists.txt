cmake_minimum_required(VERSION 3.6)

project(cpputils CXX)

add_library(cpputils INTERFACE)
add_library(cpputils::cpputils ALIAS cpputils)

# Copied from here: https://github.com/A-Ih/range-v3/blob/master/CMakeLists.txt#L44
target_include_directories(cpputils INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)

if (CPPUTILS_ENABLE_TESTING)
    set(USE_GTEST ON)
    set(USE_FMT OFF)
    set(USE_RANGEV3 ON)
    set(USE_RE2 OFF)
    set(USE_JSON OFF)
    set(USE_SPDLOG OFF)
    set(USE_ARGPARSE OFF)
    set(USE_ABSL ON)

    include(cmake/ahmad1337_deps.cmake)
    include(cmake/ahmad1337_utils.cmake)

    add_basic_executable(
        NAME test
        SRCS test/test_utils.cc
    )

    link_to_all(
        TARGETS test
        DEPS
            cpputils::cpputils
            range-v3::range-v3
            absl::strings
            absl::str_format
            absl::log
            GTest::gtest
    )
endif()
