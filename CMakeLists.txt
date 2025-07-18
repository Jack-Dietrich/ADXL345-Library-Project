cmake_minimum_required(VERSION 4.1.0)
project(adxl345googletest)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/309dab8.zip
)

add_subdirectory(src)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


enable_testing()


add_executable(
  hello_test
  hello_test.cc
)
target_link_libraries(
  hello_test
  PUBLIC adxl345
  GTest::gtest_main
)

target_include_directories(hello_test PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/src" 
                           "${PROJECT_BINARY_DIR}/src" #unsure if this is needed
                          )

include(GoogleTest)
gtest_discover_tests(hello_test)
