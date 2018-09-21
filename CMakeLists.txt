cmake_minimum_required(VERSION 3.8)

#scaler
if(NOT SCALAR)
    set(SCALAR "FLOAT")
endif()

#lower/upper scalar
string(TOUPPER ${SCALAR} SCALAR_UPPER)
string(TOLOWER ${SCALAR} SCALAR_LOWER)

#Project/exe name
set(DENN_EXECUTABLE "DENN-${SCALAR_LOWER}")

#project name
project(${DENN_EXECUTABLE})

#C++14
set(CMAKE_CXX_STANDARD 14)

#flag for all types
set(CMAKE_CXX_FLAGS "-D${SCALAR_UPPER} -fPIC -D_FORCE_INLINES")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

#Postfix (DEBUG)
set(CMAKE_DEBUG_POSTFIX "d")

#Flags
if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "/O3")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

#DENN Files
file(GLOB_RECURSE SOURCE_FILES "source/*.cpp")
file(GLOB_RECURSE HEDAER_FILES "include/*.h")

#DENN Dips
find_package(ZLIB REQUIRED)
find_package(BLAS)
find_library(M_LIB m)
find_library(UTIL_LIB util)

#Add source to target
add_executable(${DENN_EXECUTABLE} ${SOURCE_FILES})

#Add include to target
include_directories(include/)
include_directories(dips/include/)

#Add dips' include to target
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${BLAS_INCLUDE_DIRS})

#Liking
target_link_libraries(${DENN_EXECUTABLE} ${ZLIB_LIBRARIES})
target_link_libraries(${DENN_EXECUTABLE} ${M_LIB})
target_link_libraries(${DENN_EXECUTABLE} ${UTIL_LIB})

#Optional liking
if(BLAS_FOUND)
target_link_libraries(${DENN_EXECUTABLE} ${BLAS_LIBRARIES})
endif()