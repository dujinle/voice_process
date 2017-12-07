cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb -fPIC -pthread -std=c99")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g -ggdb -fPIC -pthread -std=c99")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb -fPIC -pthread -std=c99")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall -fPIC -pthread -std=c99")

project(audio-cmp)

INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/include")
FIND_LIBRARY(libsndfile "libsndfile.a" "${PROJECT_SOURCE_DIR}/libs/linux")
MESSAGE("-- find imgcodecs static lib:${libsndfile}")


ADD_SUBDIRECTORY(src)

include_directories("${PROJECT_BINARY_DIR}/include")


add_executable(audio-cmp main.c)
#target_link_libraries(audio-cmp ${libsndfile})
#target_link_libraries(audio-cmp m)
target_link_libraries(audio-cmp libprocess)
#target_link_libraries(audio-cmp libvad)
#target_link_libraries(audio-cmp libcore)
