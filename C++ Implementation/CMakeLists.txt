cmake_minimum_required(VERSION 3.10)
project(nar512)

set(CMAKE_CXX_STANDARD 11)

# Add GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.10.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Disable deprecated warnings for GoogleTest
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  add_compile_options(-Wno-deprecated-declarations)
endif()

# Add your source files here
add_executable(nar512 nar512.cpp)

# Link GoogleTest to your test executable
add_executable(nar512_test test_nar512.cpp)
target_link_libraries(nar512_test gtest_main)

include(GoogleTest)
gtest_discover_tests(nar512_test)