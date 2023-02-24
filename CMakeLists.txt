cmake_minimum_required(VERSION 3.6)

project(cpputils CXX)

add_library(cpputils INTERFACE)
add_library(cpputils::cpputils ALIAS cpputils)

# Copied from here: https://github.com/A-Ih/range-v3/blob/master/CMakeLists.txt#L44
target_include_directories(cpputils INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>)
target_include_directories(cpputils SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include/>)
