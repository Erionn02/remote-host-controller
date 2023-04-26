cmake_minimum_required(VERSION 3.20)
project(remote-host-controller)
set(CMAKE_CXX_STANDARD 23)

set(CONAN_SYSTEM_INCLUDES ON)
include(${CMAKE_SOURCE_DIR}/cmake/CMakeHelpers.cmake)

setup_conan()

add_subdirectory(src)
add_subdirectory(apps)

if (NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()
