add_library(libili9341 INTERFACE)

target_sources(libili9341 INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/ili9341.c
)

target_include_directories(libili9341 INTERFACE ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(libili9341 INTERFACE
    pico_stdlib
    pico_util
    hardware_pio
    hardware_dma
    hardware_clocks
    )

pico_generate_pio_header(libili9341 ${CMAKE_CURRENT_LIST_DIR}/ili9341.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
