cmake_minimum_required(VERSION 3.8)
project(cs5974)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES bmeans.c smpl.h smpl.c stat.c rand.c cs5974.h cs5974.c drand48.c)
add_library(cs5974 ${SOURCE_FILES})

add_executable (cs5974Output ${SOURCE_FILES})