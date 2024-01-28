add_subdirectory(${CMAKE_SOURCE_DIR}/libs/fatfs/src build)

include_directories(${CMAKE_SOURCE_DIR}/apps/pngs/include)

add_executable(tft_png
    ${CMAKE_SOURCE_DIR}/apps/pngs/hw_config.c
    ${CMAKE_SOURCE_DIR}/apps/pngs/png-help.c
    ${CMAKE_SOURCE_DIR}/apps/pngs/main.c
)

target_compile_options(tft_png PRIVATE -Wall)

# zlib dependency to use libpng
set(LIB_ZLIB ${CMAKE_SOURCE_DIR}/libs/zlib)
# set a custom build script for zlibstatic, skipping the rest
file(REMOVE ${LIB_ZLIB}/CMakeLists.txt)
file(COPY ${CMAKE_SOURCE_DIR}/libs/zlibstatic.cmake DESTINATION ${LIB_ZLIB})
file(RENAME ${LIB_ZLIB}/zlibstatic.cmake ${LIB_ZLIB}/CMakeLists.txt)
# build zlibstatic
add_subdirectory(${LIB_ZLIB} zlib_build)
target_link_libraries(tft_png PUBLIC zlibstatic)
# set libpng to use zlibstatic
set(LIB_ZCONF ${CMAKE_BINARY_DIR}/zlib_build)
include_directories(${LIB_ZLIB} ${LIB_ZCONF})
set(PNG_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON)
set(ZLIB_LIBRARIES zlibstatic)
set(ZLIB_INCLUDE_DIRS ${LIB_ZLIB} ${LIB_ZCONF})

# libpng dependencies to handle png files
set(PNG_LIB ${CMAKE_SOURCE_DIR}/libs/libpng)
# setup libpng to only build png_static, skipping the rest
set(PNG_SHARED OFF CACHE BOOL "")
set(PNG_STATIC ON CACHE BOOL "")
set(PNG_FRAMEWORK OFF CACHE BOOL "")
set(PNG_EXECUTABLES OFF CACHE BOOL "")
set(PNG_TESTS OFF CACHE BOOL "")
set(PNG_DEBUG OFF CACHE BOOL "")
include_directories(${PNG_LIB})
add_subdirectory(${PNG_LIB} ${CMAKE_SOURCE_DIR}/build/libpng)
configure_file(${PNG_LIB}/scripts/pnglibconf.h.prebuilt ${PNG_LIB}/pnglibconf.h)

target_link_libraries(tft_png PUBLIC
    pico_stdlib
    libili9341
    no-OS-FatFS-SD-SDIO-SPI-RPi-Pico
    png_static)

pico_enable_stdio_usb(tft_png 1) # enable usb output
pico_enable_stdio_uart(tft_png 0) # disable uart output

pico_add_extra_outputs(tft_png)
