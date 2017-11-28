cmake_minimum_required(VERSION 3.8)
project(DENN_LITE)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "-DFLOAT -fPIC -D_FORCE_INLINES")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

set(CMAKE_CXX_FLAGS_DEBUG "-g -D_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")


include_directories(include/)
include_directories(dips/include/)

file(GLOB denn_source source/*.cpp source/**/*.cpp source/**/**/*.cpp)
SET(SOURCE_FILES ${denn_source})

find_package(ZLIB)
find_library(M_LIB m)
find_library(UTIL_LIB util)

if(ZLIB_FOUND)
    add_executable(DENN_LITE ${SOURCE_FILES})
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(DENN_LITE ${ZLIB_LIBRARIES})
    target_link_libraries(DENN_LITE ${M_LIB})
    target_link_libraries(DENN_LITE ${UTIL_LIB})
else()
    message(FATAL_ERROR "Zlib not found")
endif()