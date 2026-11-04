cmake_minimum_required(VERSION 3.14)
project(stack_project VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------- Основной исполняемый файл ----------
# include/stack.hpp содержит только определение класса TStack<T>,
# а реализация методов находится в src/main.cpp (подключается через #include,
# так как тела шаблонных методов должны быть видны в точке инстанцирования).
# Точка входа — src/app_main.cpp; src/main.cpp попадает в сборку
# автоматически через #include в include/stack.hpp.
add_executable(stack_app src/app_main.cpp)
target_include_directories(stack_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

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

    add_executable(stack_tests tests/stack_test.cpp)
    target_include_directories(stack_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(stack_tests PRIVATE GTest::gtest)

    include(GoogleTest)
    gtest_discover_tests(stack_tests)
endif()
