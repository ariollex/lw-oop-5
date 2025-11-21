cmake_minimum_required(VERSION 3.10)

project(2025_LW_OOP_5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Install Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.17.0
  TLS_VERIFY false
)
FetchContent_MakeAvailable(googletest)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${INCLUDE_DIR})

# Build
add_library(lab5_lib src/CustomMemoryResource.cpp)
add_executable(lab5_exe main.cpp)
target_link_libraries(lab5_exe PRIVATE lab5_lib)

# Build Tests
enable_testing()
add_executable(lab5_tests test/tests.cpp)
target_link_libraries(lab5_tests lab5_lib gtest_main)
