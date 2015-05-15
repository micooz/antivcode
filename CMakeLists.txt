cmake_minimum_required(VERSION 2.8.4)
project(antivcode)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(include)
include_directories(ProgramOptions/include)
include_directories(filesystem/include)

set(SOURCE_FILES
    src/Bitmap.cpp
    src/Color.cpp
    src/Database.cpp
    src/Decoder.cpp
    src/IImage.cpp
    src/Jpeg.cpp
    src/main.cpp
    src/Slice.cpp
    src/Study.cpp
)

# find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
# find_library(LIBJPEG NAMES jpeg HINTS jpeg-9a)
# find_library(LIBPROGRAMOPTIONS NAMES program_options HINTS ProgramOptions)
add_subdirectory(ProgramOptions)
# find_library(LIB_PROGRAM_OPTIONS NAMES program_options HINTS ProgramOptions)

add_executable(antivcode ${SOURCE_FILES})
# target_link_libraries (antivcode ${Boost_LIBRARIES} ${LIBJPEG})
target_link_libraries (antivcode program_options)
