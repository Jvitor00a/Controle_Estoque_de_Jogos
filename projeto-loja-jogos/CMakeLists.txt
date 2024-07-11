cmake_minimum_required(VERSION 3.10)

project(LojaJogos VERSION 0.0.1)

set(CMAKE_C_STANDARD 99)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB all_SRCS
        "${PROJECT_SOURCE_DIR}/include/*.h"
        "${PROJECT_SOURCE_DIR}/src/*.c"
        )

set(CMAKE_BUILD_TYPE Debug)
        
add_executable(LojaJogos ${all_SRCS})
