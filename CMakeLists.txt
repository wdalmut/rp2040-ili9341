# version requirements
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# target configuration
set(CMAKE_C_COMPILER /usr/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /usr/bin/arm-none-eabi-g++)

# initialization
include(${CMAKE_SOURCE_DIR}/pico-sdk/pico_sdk_init.cmake)

# project definition
project(rp2040-tft)

pico_sdk_init()
add_compile_options(-Wall)

add_subdirectory(src)

add_subdirectory(apps)