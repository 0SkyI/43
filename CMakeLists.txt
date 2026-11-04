cmake_minimum_required(VERSION 3.14)
project(stack_project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Основной исполняемый файл ----------
# Весь класс TStack<T> (определение + реализация) и демо находятся в main.cpp.
add_executable(stack_app main.cpp)

# ---------- Тесты GoogleTest ----------
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # Для Windows: не перекрывать стандартные настройки компилятора проекта
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    # tests/stack_test.cpp подключает реализацию через #include "../main.cpp",
    # поэтому собирается как единственный трансляционный блок.
    add_executable(stack_tests tests/stack_test.cpp)
    target_link_libraries(stack_tests PRIVATE GTest::gtest)

    include(GoogleTest)
    gtest_discover_tests(stack_tests)
endif()
