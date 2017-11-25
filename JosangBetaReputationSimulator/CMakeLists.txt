cmake_minimum_required(VERSION 3.8)
project(cs5974_JBR)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES bmeans.c smpl.h smpl.c stat.c rand.c cs5974_JBR.h cs5974_JBR.c drand48.c)
add_library(cs5974_JBR ${SOURCE_FILES})

add_executable (cs5974_JBROutput ${SOURCE_FILES})