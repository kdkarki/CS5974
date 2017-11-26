cmake_minimum_required(VERSION 3.8)
project(NoTrustScore)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES bmeans.c smpl.h smpl.c stat.c rand.c  CS5974_NTS.h CS5974_NTS.c drand48.c)
add_library(NoTrustScore ${SOURCE_FILES})

add_executable (cs5974_NTS ${SOURCE_FILES})