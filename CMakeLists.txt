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

include_directories(
    ${CMAKE_SOURCE_DIR}/src/include
)

add_executable(${PROJECT_NAME}
    src/ili9341.c
    src/main.c
)

pico_generate_pio_header(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/src/ili9341.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/src/include)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    pico_stdlib
    pico_util
    hardware_pio
    hardware_dma
    hardware_clocks
    )

pico_enable_stdio_usb(${PROJECT_NAME} 1) # enable usb output
pico_enable_stdio_uart(${PROJECT_NAME} 0) # disable uart output

pico_add_extra_outputs(${PROJECT_NAME}) # create map/bin/hex/uf2 files in addition to elf