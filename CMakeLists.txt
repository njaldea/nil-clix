cmake_minimum_required(VERSION 3.27.1)

project(nil-clix VERSION 1.0.2 LANGUAGES CXX)

include(cmake/package.cmake)
include(cmake/toolchain.cmake)
include(cmake/quality.cmake)
include(cmake/test.cmake)
include(cmake/coverage.cmake)
include(cmake/cpack.cmake)

add_subdirectory(src)
add_subdirectory(sandbox)