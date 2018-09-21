cmake_minimum_required(VERSION 3.8)
project(DENN_LITE)

set(CMAKE_CXX_STANDARD 14)

#scaler
if(NOT SCALAR)
    set(SCALAR "FLOAT")
endif()

#lower/upper scalar
string(TOUPPER ${SCALAR} SCALAR_UPPER)
string(TOLOWER ${SCALAR} SCALAR_LOWER)

#flag for all types
set(CMAKE_CXX_FLAGS "-D${SCALAR_UPPER} -fPIC -D_FORCE_INLINES")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)

#POSTFIX FOR DEBUG
if(NOT CMAKE_DEBUG_POSTFIX)
    set(CMAKE_DEBUG_POSTFIX d)
endif()

#flags
if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "/O3")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
  set(CMAKE_CXX_FLAGS_RELEASE "-O3")
endif()

#DENN FILES
file(GLOB_RECURSE SOURCE_FILES "source/*.cpp")
file(GLOB_RECURSE HEDAER_FILES "include/*.h")

#DENN DIPS
find_package(ZLIB REQUIRED)
find_package(BLAS)
find_library(M_LIB m)
find_library(UTIL_LIB util)

#SET OUTPUT NAME
set(DENN_EXECUTABLE "DENN-${SCALAR_LOWER}")

#ADD SOUCE TO DENN TARGET
add_executable(${DENN_EXECUTABLE} ${SOURCE_FILES})

#ADD INCLUDE TO DENN TARGET
include_directories(include/)
include_directories(dips/include/)

#ADD DIPS INCLUDE
include_directories(${ZLIB_INCLUDE_DIRS})
include_directories(${BLAS_INCLUDE_DIRS})

#LIKING DIPS
target_link_libraries(${DENN_EXECUTABLE} ${ZLIB_LIBRARIES})
target_link_libraries(${DENN_EXECUTABLE} ${M_LIB})
target_link_libraries(${DENN_EXECUTABLE} ${UTIL_LIB})

#OPTIONAL LIKING
if(BLAS_FOUND)
target_link_libraries(${DENN_EXECUTABLE} ${BLAS_LIBRARIES})
endif()