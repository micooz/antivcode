cmake_minimum_required(VERSION 2.8.4)
project(antivcode)

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

set(BOOST_LIBS
    program_options
    regex
    system
    filesystem
)

find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_library(LIBJPEG NAMES jpeg HINTS ../jpeg-9a)

add_executable(antivcode ${SOURCE_FILES})
target_link_libraries (antivcode ${Boost_LIBRARIES} ${LIBJPEG})
