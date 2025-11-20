cmake_minimum_required(VERSION 3.15)
project(SingleLinkedListProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опции компиляции
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Основная программа
add_executable(main
    src/main.cpp
    include/allocator.h
    include/list.h
)

# Google Test - автоматическое скачивание если не найден
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

# Для Windows может потребоваться установка Windows SDK
if(WIN32)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

enable_testing()

# Тесты
add_executable(test_list
    tests/test_list.cpp
    include/allocator.h
    include/list.h
)

target_link_libraries(test_list GTest::gtest GTest::gtest_main)

# Добавляем тесты
add_test(NAME SingleLinkedListTests COMMAND test_list)

# Дополнительная цель для удобного запуска тестов
add_custom_target(run_tests
    COMMAND test_list
    DEPENDS test_list
    COMMENT "Running tests..."
)