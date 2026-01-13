cmake_minimum_required(VERSION 3.27.1)

project(
    nil-clix
    VERSION 1.0.4
    LANGUAGES CXX
    DESCRIPTION "Opinionated Argument Parser"
)

find_package(nil-cmake CONFIG REQUIRED)

install_config("cmake/config.cmake.in")

add_subdirectory(src)
add_subdirectory(sandbox)