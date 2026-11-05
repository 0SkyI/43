cmake_minimum_required(VERSION 3.14)
project(stack_project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(stack_app tstack.cpp)

# GoogleTest 
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    add_executable(stack_tests tests/stack_test.cpp)
    target_link_libraries(stack_tests PRIVATE GTest::gtest)

    include(GoogleTest)
    gtest_discover_tests(stack_tests)
endif()
