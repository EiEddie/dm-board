cmake_minimum_required(VERSION 3.15)
project(components NONE)


set(COMPONENTS_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
set(COMPONENTS_PUBLIC_INCLUDE_DIR ${CMAKE_BINARY_DIR}/components_public_include)


# 创建 public include 目录
file(MAKE_DIRECTORY ${COMPONENTS_PUBLIC_INCLUDE_DIR})

# 找到 Components 下的所有子目录
file(GLOB CHILDREN RELATIVE ${COMPONENTS_ROOT} "${COMPONENTS_ROOT}/*")
set(FOUND_COMPONENTS)
foreach(entry ${CHILDREN})
	if(IS_DIRECTORY "${COMPONENTS_ROOT}/${entry}")
		list(APPEND FOUND_COMPONENTS ${entry})
	endif()
endforeach()

# 把每个组件的 include/* 复制到 build/components_public_include/comp/<name>
foreach(comp ${FOUND_COMPONENTS})
	set(src_include "${COMPONENTS_ROOT}/${comp}/include")
	if(EXISTS "${src_include}")
		set(dest "${COMPONENTS_PUBLIC_INCLUDE_DIR}/comp/${comp}")
		file(MAKE_DIRECTORY ${dest})
		# 在 configure 阶段复制整个目录
		if(EXISTS "${src_include}")
			file(COPY "${src_include}/" DESTINATION "${dest}")
		endif()
	endif()
	message(STATUS "Found components: ${comp}")
endforeach()

# 创建一个 INTERFACE target 暴露 public include
add_library(comp_public_includes INTERFACE)
target_include_directories(comp_public_includes INTERFACE
	$<BUILD_INTERFACE:${COMPONENTS_PUBLIC_INCLUDE_DIR}>
	$<INSTALL_INTERFACE:include>
)

# 使用 CMakeLists.txt 进行组件自管理
foreach(comp ${FOUND_COMPONENTS})
	set(comp_dir "${COMPONENTS_ROOT}/${comp}")
	add_subdirectory("${comp_dir}")
endforeach()

message(STATUS "Public include dir: ${COMPONENTS_PUBLIC_INCLUDE_DIR}")
message(STATUS "Access components via targets: comp::<name>")
