cmake_minimum_required(VERSION 2.8.4)
project(antivcode)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)
include_directories(vendor/ProgramOptions/include)
include_directories(vendor/jpeg-9a)

aux_source_directory(src SOURCE_FILES)

add_subdirectory(vendor/ProgramOptions)

find_package(Boost COMPONENTS filesystem system REQUIRED)

find_library(LIBJPEG NAMES jpeg HINTS jpeg-9a)
add_executable(antivcode ${SOURCE_FILES})

target_link_libraries (antivcode ${Boost_LIBRARIES} ${LIBJPEG})
target_link_libraries(antivcode program_options jpeg)
