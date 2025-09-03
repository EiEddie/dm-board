project(bsp LANGUAGES C CXX)

file(GLOB_RECURSE BSP_SOURCES
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc"
)

add_library(bsp STATIC ${BSP_SOURCES})

target_include_directories(bsp
	PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_sources(bsp PRIVATE ${BSP_SOURCES})
