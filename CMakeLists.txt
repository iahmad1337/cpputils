cmake_minimum_required(VERSION 3.6)

project(cpputils CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(
    cpputils
    src/string.cc
)
target_include_directories(cpputils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(cpputils::cpputils ALIAS cpputils)

if (CPPUTILS_ENABLE_TESTING)
    set(USE_GTEST ON)

    include(cmake/ahmad1337_deps.cmake)
    include(cmake/ahmad1337_utils.cmake)

    add_basic_executable(
        NAME test_string_utils
        SRCS test/test_string_utils.cc
    )

    add_basic_executable(
        NAME test_itertools
        SRCS test/test_itertools.cc
    )

    add_basic_executable(
        NAME test_meta
        SRCS test/test_meta.cc
    )

    add_basic_executable(
        NAME test_reflect
        SRCS test/test_reflect.cc
    )

    link_to_all(
        TARGETS
            test_string_utils
            test_itertools
            test_meta
            test_reflect
        DEPS
            cpputils::cpputils
            GTest::gtest_main
    )
endif()
