cmake_minimum_required(VERSION 3.12)

add_executable(
        unit_tests
        test.cpp)

target_link_libraries(
        unit_tests
        gtest_main
)

add_test(
        NAME
        unit
        COMMAND
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unit_tests
)