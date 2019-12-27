cmake_minimum_required(VERSION 3.13)
project(interpreter)

set(CMAKE_CXX_STANDARD 14)

if(MINGW)
    set(GTEST_DISABLE_PTHREADS ON)
endif()

if(WIN32)
    set(WINDOWS_CMAKE_ARGS
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=DebugLibs
            -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=ReleaseLibs
            )
else(WIN32)
    set(WINDOWS_CMAKE_ARGS )
endif(WIN32)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

add_subdirectory(src)
target_include_directories(interpreter PUBLIC lib)
target_include_directories(parser PUBLIC lib)

enable_testing()
add_subdirectory(testing)

target_include_directories(unit_tests PUBLIC lib)