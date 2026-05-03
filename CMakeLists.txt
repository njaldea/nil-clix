cmake_minimum_required(VERSION 3.27.1)

project(
    nil-clix
    VERSION 1.0.10
    LANGUAGES CXX
    DESCRIPTION "Opinionated Argument Parser"
)

find_package(nil-cmake CONFIG REQUIRED)

set(ENABLE_C_API OFF CACHE BOOL "[0 | OFF - 1 | ON]: build c api?")

install_config("cmake/config.cmake.in")

add_subdirectory(src)
add_subdirectory(sandbox)