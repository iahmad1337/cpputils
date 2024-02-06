cmake_minimum_required(VERSION 3.6)

project(cpputils CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(
    cpputils
    src/string.cc
)
target_include_directories(cpputils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(cpputils::cpputils ALIAS cpputils)

if (CPPUTILS_ENABLE_TESTING)
    set(USE_GTEST ON)
    set(USE_RANGEV3 ON)
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
