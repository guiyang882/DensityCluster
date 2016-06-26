cmake_minimum_required(VERSION 3.2)
project(DensityCluster)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp")

ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

set(SOURCE_FILES main.cpp densitycluster.cpp densitycluster.h)
add_executable(DensityCluster ${SOURCE_FILES})