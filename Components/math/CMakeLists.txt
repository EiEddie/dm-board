add_library(comp_math STATIC
	src/comp_filter.c
	src/lowpass_filter.c
)

target_include_directories(comp_math
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/comp/math>
)

target_link_libraries(comp_math PUBLIC comp_public_includes)
target_link_libraries(comp_math PUBLIC m)

add_library(comp::math ALIAS comp_math)
