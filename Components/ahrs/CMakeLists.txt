add_library(comp_ahrs STATIC
	src/ahrs.c
)

target_include_directories(comp_ahrs
	PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include/comp/ahrs>
)

target_link_libraries(comp_ahrs PUBLIC comp_public_includes)
# 依赖
target_link_libraries(comp_ahrs PUBLIC comp::math)

add_library(comp::ahrs ALIAS comp_ahrs)
