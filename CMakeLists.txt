cmake_minimum_required(VERSION 3.12)

# Project name and version
project(c0 VERSION 0.0.1)

# Compiler options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

add_subdirectory(3rd_party/fmt)
add_subdirectory(3rd_party/argparse)

# Build libc0
set(LIB "lib${PROJECT_NAME}")
set(LIB_SRC
        src/lexer/lexer.cpp
        src/lexer/lexer.h
        src/analyser/analyser.cpp
        src/analyser/analyser.h
        src/error/error.h
        src/core.h src/lexer/token.h)
add_library(${LIB} ${LIB_SRC})
target_include_directories(${LIB} PRIVATE ./src)

# Build executable
set(EXE ${PROJECT_NAME})
set(EXE_SRC
        src/main.cpp)
add_executable(${EXE} ${EXE_SRC})
target_include_directories(${EXE} PRIVATE ./src)

if(MSVC)
    target_compile_options(${EXE} PRIVATE /W3)
    target_compile_options(${LIB} PRIVATE /W3)
else()
    target_compile_options(${EXE} PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(${LIB} PRIVATE -Wall -Wextra -pedantic)
endif()

target_link_libraries(${EXE} ${LIB} fmt argparse)

# Test
add_subdirectory(3rd_party/catch2)
enable_testing()

set(TEST "${PROJECT_NAME}_test")
set(TEST_SRC
        tests/test_all.cpp
        tests/test_lexer.cpp
        tests/test_analyser.cpp)
add_executable(${TEST} ${TEST_SRC})
target_include_directories(${TEST} PRIVATE .)
target_link_libraries(${TEST} Catch2 ${LIB} fmt)
add_test(all_test ${TEST})
find_program(OPEN_CPP_COVERAGE OpenCppCoverage.exe)

if (MSVC AND OPEN_CPP_COVERAGE)
    string(REPLACE "/" "\\" MY_SOURCE_DIR "${CMAKE_SOURCE_DIR}")
    add_custom_command(TARGET miniplc0_test
            POST_BUILD
            COMMAND ${OPEN_CPP_COVERAGE}
            --sources ${MY_SOURCE_DIR}
            --excluded_sources ${MY_SOURCE_DIR}\\3rd_party
            "$<TARGET_FILE:miniplc0_test>")
else()
endif()
