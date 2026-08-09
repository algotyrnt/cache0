cmake_minimum_required(VERSION 3.14)
project(Cache0 VERSION 1.0.0 LANGUAGES CXX)

# 1. Define Cache0 as a Header-Only (INTERFACE) library
add_library(cache0 INTERFACE)
target_include_directories(cache0 INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>$<INSTALL_INTERFACE:include>
)
target_compile_features(cache0 INTERFACE cxx_std_17)

# 2. Setup Tests
enable_testing()
add_executable(run_tests tests/test_basic.cpp)
target_link_libraries(run_tests PRIVATE cache0)
add_test(NAME BasicTests COMMAND run_tests)

# 3. Setup Demo
add_executable(demo examples/demo.cpp)
target_link_libraries(demo PRIVATE cache0)
