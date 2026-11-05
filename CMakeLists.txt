cmake_minimum_required(VERSION 3.16)
project(sec5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(sec5_core src/includes.cpp)
target_include_directories(sec5_core PUBLIC src)

add_executable(sec5 src/main.cpp)
target_link_libraries(sec5 PRIVATE sec5_core)

include(CTest)
if(BUILD_TESTING)
    find_package(GTest QUIET)
    if(NOT TARGET GTest::gtest_main)
        include(FetchContent)
        if(POLICY CMP0135)
            cmake_policy(SET CMP0135 NEW)
        endif()
        FetchContent_Declare(
            googletest
            URL https://codeload.github.com/google/googletest/tar.gz/refs/tags/v1.15.2
            URL_HASH SHA256=7b42b4d6ed48810c5362c265a17faebe90dc2373c885e5216439d37927f02926
        )
        FetchContent_MakeAvailable(googletest)
    endif()

    add_executable(sec5_tests tests/includes_test.cpp)
    target_link_libraries(sec5_tests PRIVATE sec5_core GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(sec5_tests)
    if(UNIX)
        add_test(NAME sec5_cli
                 COMMAND sh "${CMAKE_CURRENT_SOURCE_DIR}/tests/check_cli.sh" $<TARGET_FILE:sec5>)
    endif()
endif()
