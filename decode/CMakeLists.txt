cmake_minimum_required(VERSION 2.8.4)
project(decode)

set(SOURCE_FILES
    Bitmap.cpp
    Color.cpp
    Database.cpp
    Decoder.cpp
    IImage.cpp
    Jpeg.cpp
    main.cpp
    Slice.cpp
    Study.cpp
)

set(BOOST_LIBS
    program_options
    regex
    system
    filesystem
)
find_package(Boost COMPONENTS ${BOOST_LIBS} REQUIRED)
find_library(LIBJPEG NAMES jpeg HINTS ../jpeg-6b)

add_executable(decode ${SOURCE_FILES})
target_link_libraries (decode ${Boost_LIBRARIES} ${LIBJPEG})
