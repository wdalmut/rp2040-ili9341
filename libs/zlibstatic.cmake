cmake_minimum_required(VERSION 3.13)
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

project(zlib C)

set(VERSION "1.2.13")

set(ZLIB_SRC ${CMAKE_SOURCE_DIR}/libs/zlib)

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckCSourceCompiles)
enable_testing()

check_include_file(sys/types.h HAVE_SYS_TYPES_H)
check_include_file(stdint.h    HAVE_STDINT_H)
check_include_file(stddef.h    HAVE_STDDEF_H)

#
# Check to see if we have large file support
#
set(CMAKE_REQUIRED_DEFINITIONS -D_LARGEFILE64_SOURCE=1)
# We add these other definitions here because CheckTypeSize.cmake
# in CMake 2.4.x does not automatically do so and we want
# compatibility with CMake 2.4.x.
if(HAVE_SYS_TYPES_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_SYS_TYPES_H)
endif()
if(HAVE_STDINT_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDINT_H)
endif()
if(HAVE_STDDEF_H)
    list(APPEND CMAKE_REQUIRED_DEFINITIONS -DHAVE_STDDEF_H)
endif()
check_type_size(off64_t OFF64_T)
if(HAVE_OFF64_T)
   add_definitions(-D_LARGEFILE64_SOURCE=1)
endif()
set(CMAKE_REQUIRED_DEFINITIONS) # clear variable

#
# Check for fseeko
#
check_function_exists(fseeko HAVE_FSEEKO)
if(NOT HAVE_FSEEKO)
    add_definitions(-DNO_FSEEKO)
endif()

#
# Check for unistd.h
#
check_include_file(unistd.h Z_HAVE_UNISTD_H)

set(ZLIB_PC ${ZLIB_SRC}/zlib.pc)
configure_file(${ZLIB_SRC}/zlib.pc.cmakein ${ZLIB_PC} @ONLY)
configure_file(${ZLIB_SRC}/zconf.h.cmakein ${ZLIB_SRC}/zconf.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_SOURCE_DIR})

#============================================================================
# zlib
#============================================================================

set(ZLIB_PUBLIC_HDRS
    ${ZLIB_SRC}/zconf.h
    ${ZLIB_SRC}/zlib.h
)
set(ZLIB_PRIVATE_HDRS
    ${ZLIB_SRC}/crc32.h
    ${ZLIB_SRC}/deflate.h
    ${ZLIB_SRC}/gzguts.h
    ${ZLIB_SRC}/inffast.h
    ${ZLIB_SRC}/inffixed.h
    ${ZLIB_SRC}/inflate.h
    ${ZLIB_SRC}/inftrees.h
    ${ZLIB_SRC}/trees.h
    ${ZLIB_SRC}/zutil.h
)
set(ZLIB_SRCS
    ${ZLIB_SRC}/adler32.c
    ${ZLIB_SRC}/compress.c
    ${ZLIB_SRC}/crc32.c
    ${ZLIB_SRC}/deflate.c
    ${ZLIB_SRC}/gzclose.c
    ${ZLIB_SRC}/gzlib.c
    ${ZLIB_SRC}/gzread.c
    ${ZLIB_SRC}/gzwrite.c
    ${ZLIB_SRC}/inflate.c
    ${ZLIB_SRC}/infback.c
    ${ZLIB_SRC}/inftrees.c
    ${ZLIB_SRC}/inffast.c
    ${ZLIB_SRC}/trees.c
    ${ZLIB_SRC}/uncompr.c
    ${ZLIB_SRC}/zutil.c
)

add_library(zlibstatic STATIC ${ZLIB_SRCS} ${ZLIB_PUBLIC_HDRS} ${ZLIB_PRIVATE_HDRS})