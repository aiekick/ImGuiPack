cmake_minimum_required(VERSION 3.1)

set(PROJECT ImGuiPackApp)

enable_language(C CXX)
project(${PROJECT} CXX)

## for group smake targets in the dir cmakeTargets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmakeTargets")
set_property(GLOBAL PROPERTY GLOBAL_DEPENDS_DEBUG_MODE 1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

set(FINAL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}/debug")

set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_INSTALL_BINDIR .)

set(USE_SHARED_LIBS ON CACHE BOOL "" FORCE)

set(LLVM_USE_CRT_DEBUG MDd CACHE STRING "" FORCE)
set(LLVM_USE_CRT_MINSIZEREL MD CACHE STRING "" FORCE)
set(LLVM_USE_CRT_RELEASE MD CACHE STRING "" FORCE)
set(LLVM_USE_CRT_RELWITHDEBINFO MD CACHE STRING "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "")

if ("${CMAKE_EXE_LINKER_FLAGS}" STREQUAL "/machine:x64")
	set(ARCH x64)
else()
	set(ARCH x32)
endif()

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX _debug)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /FR /Zo")
    add_definitions(-D_USE_MATH_DEFINES)
    add_definitions(-DMSVC)
	add_definitions(-DNOMINMAX) ## for avoid issue regarding std::max and std::min
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wunused-parameter -Wno-unknown-pragmas")
endif ()

include(cmake/3rdparty.cmake)

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if (USE_NEW_IMGUI_KEY_SYSTEM)
	add_definitions(-DIMGUI_DISABLE_OBSOLETE_KEYIO)
endif()

file(GLOB MAIN_SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)
source_group(main FILES ${IMGUI_IMPL_SRC})

file(GLOB_RECURSE SRC_SOURCES ${CMAKE_SOURCE_DIR}/src/*.*)
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX src FILES ${SRC_SOURCES})

file(GLOB_RECURSE PROJECT_CMAKE_FILES
		${CMAKE_SOURCE_DIR}/cmake/*.cmake)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/cmake PREFIX cmake FILES ${PROJECT_CMAKE_FILES})

file(GLOB IMGUI_IMPL_SOURCES 
		${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_glfw.cpp
		${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_glfw.h
		${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
		${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui/backends/imgui_impl_opengl3.h)
source_group(src\\ImGuiImpl FILES ${IMGUI_IMPL_SOURCES})

if (APPLE)
	##set(ICON icon.icns)
	##set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

include_directories(
	${GLM_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}
	${PLOG_INCLUDE_DIR}
	${GLFW_INCLUDE_DIR}
	${GLAD_INCLUDE_DIR}
	${IMGUI_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${IMPLOT_INCLUDE_DIR}
	${IMGUIPACK_INCLUDE_DIRS}
)

add_executable(${PROJECT}
	##${ICON}
	MACOSX_BUNDLE
	${SRC_SOURCES}
	${MAIN_SOURCES}
	${IMGUI_IMPL_SOURCES}
	${PROJECT_CMAKE_FILES}
)

if (USE_SHARED_LIBS)
	target_compile_definitions(${PROJECT} PRIVATE BUILD_IMGUI_PACK_SHARED_LIBS)
endif()

if (APPLE)
	set_target_properties(${PROJECT}
			PROPERTIES
			OUTPUT_NAME "${PROJECT}_Macos"
			MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT}"
			MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT}"
			FOLDER "${PROJECT}"
			##RESOURCE icon.icns
			MACOSX_BUNDLE_SHORT_VERSION_STRING ""
			MACOSX_BUNDLE_LONG_VERSION_STRING ""
			##MACOSX_BUNDLE_ICON_FILE icon.icns
			MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/MacOSXBundleInfo.plist.in")
elseif(WIN32)
	set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_WIn32")
elseif(UNIX)
	set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_Unix")
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})

target_link_libraries(${PROJECT} PRIVATE
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLAD_LIBRARIES}
	${IMGUIPACK_LIBRARIES}
)

target_include_directories(${PROJECT} PRIVATE
	${CMAKE_SOURCE_DIR}
	${IMGUIPACK_INCLUDE_DIR}
)

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
