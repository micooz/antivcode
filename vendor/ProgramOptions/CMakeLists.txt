cmake_minimum_required(VERSION 2.8.4)
project(program_options)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include/)

# library sources
set(SOURCE_FILES
    src/parser/parser.cc
    src/parser/error.cc
    src/parser/item.cc
    src/generator/generator.cc
    src/generator/subroutine.cc
    src/generator/row.cc
)

# create a static link library
add_library(program_options STATIC ${SOURCE_FILES})

# gtest
add_subdirectory(googletest)
include_directories(googletest/include/)
enable_testing()

# create executables linked with libparser.a
add_executable(parser_test      test/parser_test.cc)
add_executable(generator_test   test/generator_test.cc)
add_executable(combination_test test/combination_test.cc)

target_link_libraries(parser_test       program_options gtest)
target_link_libraries(generator_test    program_options gtest)
target_link_libraries(combination_test  program_options gtest)