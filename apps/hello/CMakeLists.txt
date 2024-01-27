add_executable(hello_tft
	main.c
)

target_compile_options(hello_tft PRIVATE -Wall)

target_link_libraries(hello_tft
	pico_stdlib
	pico_util
	libili9341
)

# create map/bin/hex file etc.
pico_add_extra_outputs(hello_tft)