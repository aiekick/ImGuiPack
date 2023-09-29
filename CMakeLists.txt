cmake_minimum_required(VERSION 3.5)

set(PROJECT ImGuiPack)
set(PROJECT_VER 1.89.9)

enable_language(C CXX)

project(
	${PROJECT} 
	VERSION ${PROJECT_VER} 
	LANGUAGES CXX)

option(USE_IMPLOT "Use ImPlot" OFF)
option(USE_IMCOOL_BAR "ImCoolBar" OFF)
option(USE_IM_GRADIENT_HDR "ImGradientHDR" OFF)
option(USE_IMGUI_FILE_DIALOG "ImGuiFileDialog" OFF)
option(IMGUIPACK_USE_STD_FILESYSTEM "Enable the use of std::filesystem in ImGuiFileDialog" OFF)
option(USE_IM_GUIZMO "ImGuizmo" OFF)
option(USE_IM_TOOLS "ImTools" OFF)
option(USE_IMGUI_NODE_EDITOR "ImGuiNodeEditor" OFF)
option(USE_IMGUI_MARKDOW "ImGuiMarkDown" OFF)
option(USE_IMGUI_COLOR_TEXT_EDIT "Use ImGuiColorTextEdit" OFF)
option(USE_IN_APP_GPU_PROFILER "Use In App Gpu Prtofiler" OFF)

if(USE_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
	set(LLVM_USE_CRT_DEBUG MDd CACHE STRING "" FORCE)
	set(LLVM_USE_CRT_MINSIZEREL MD CACHE STRING "" FORCE)
	set(LLVM_USE_CRT_RELEASE MD CACHE STRING "" FORCE)
	set(LLVM_USE_CRT_RELWITHDEBINFO MD CACHE STRING "" FORCE)
	set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
else()
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(LLVM_USE_CRT_DEBUG MTd CACHE STRING "" FORCE)
	set(LLVM_USE_CRT_MINSIZEREL MT CACHE STRING "" FORCE)
	set(LLVM_USE_CRT_RELEASE MT CACHE STRING "" FORCE)
	set(LLVM_USE_CRT_RELWITHDEBINFO MT CACHE STRING "" FORCE)
	set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
endif()

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

#####################################################
##### FREETYPE
#####################################################

set(FREETYPE_LIBRARIES freetype)
set(FT_WITH_PNG OFF CACHE BOOL "" FORCE)
set(FT_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(FT_WITH_BZIP2 OFF CACHE BOOL "" FORCE)
set(FT_WITH_BROTLI OFF CACHE BOOL "" FORCE)
set(FT_WITH_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(FREETYPE_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/freetype2/include)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/freetype2 EXCLUDE_FROM_ALL)

if(USE_SHARED_LIBS)
	set_target_properties(freetype PROPERTIES FOLDER 3rdparty/Shared)
else()
	set_target_properties(freetype PROPERTIES FOLDER 3rdparty/Static)
endif()

#####################################################
#####################################################

## for group smake targets in the dir cmakeTargets
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "cmakeTargets")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

add_definitions(-DUSE_STD_FILESYSTEM)
add_definitions(-DFT_DEBUG_LEVEL_TRACE)
add_definitions(-DIMGUI_ENABLE_FREETYPE)
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
#add_definitions(-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)
add_definitions(-DIMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/CustomImConfig.h")

#####################################################
##### IMGUI
#####################################################

file(GLOB IMGUI_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/misc/freetype/imgui_freetype.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/misc/freetype/imgui_freetype.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_SOURCES})

#####################################################
##### IMPLOT
#####################################################

if (USE_IMPLOT)
	file(GLOB IMGUI_IMPLOT_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMPLOT_SOURCES})
endif()

#####################################################
##### IMCOOLBAR
#####################################################

if (USE_IMCOOL_BAR)
	file(GLOB IMGUI_IMCOOLBAR_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMCOOLBAR_SOURCES})
endif()

#####################################################
##### IMGRADIENTHDR
#####################################################

if (USE_IM_GRADIENT_HDR)
	file(GLOB IMGUI_IMGRADIENTHDR_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/src/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/src/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGRADIENTHDR_SOURCES})
endif()

#####################################################
##### IMGUIFILEDIALOG
#####################################################

if (USE_IMGUI_FILE_DIALOG)
	file(GLOB IMGUI_IMGUIFILEDIALOG_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIFILEDIALOG_SOURCES})
endif()

#####################################################
##### IMGUIZMO
#####################################################

if (USE_IM_GUIZMO)
	file(GLOB IMGUI_IMGUIZMO_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIZMO_SOURCES})
endif()

#####################################################
##### IMTOOLS
#####################################################

if (USE_IM_TOOLS)
	file(GLOB IMGUI_IMTOOLS_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMTOOLS_SOURCES})
endif()

#####################################################
##### IMGUINODEEDITOR
#####################################################

if (USE_IMGUI_NODE_EDITOR)
	file(GLOB IMGUI_IMGUINODEEDITOR_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/*.inl
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUINODEEDITOR_SOURCES})
endif()

#####################################################
##### IMGUIMARKDONW
#####################################################

if (USE_IMGUI_MARKDOW)
	file(GLOB IMGUI_IMGUIMARKDONW_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_markdown/*.h)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIMARKDONW_SOURCES})
endif()

#####################################################
##### IMGUICOLORTEXTEDIT
#####################################################

if (USE_IMGUI_COLOR_TEXT_EDIT)
	file(GLOB IMGUI_IMGUICOLORTEXTEDIT_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguicolottextedit/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguicolottextedit/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUICOLORTEXTEDIT_SOURCES})
endif()

#####################################################
##### INAPPGPUPROFILER
#####################################################

if (USE_IN_APP_GPU_PROFILER)
	file(GLOB IMGUI_INAPPGPUPROFILER_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/inappgpuprofiler/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/inappgpuprofiler/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_INAPPGPUPROFILER_SOURCES})
endif()

#####################################################
##### IMGUIPACK
#####################################################

file(GLOB IMGUI_IMGUIPACK_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ PREFIX header FILES ${IMGUI_IMGUIPACK_SOURCES})

#####################################################
#####################################################
#####################################################

if (BUILD_SHARED_LIBS)
	set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE BOOL "" FORCE)
	add_library(${PROJECT} 
		${IMGUI_SOURCES} 
		${IMGUI_IMPLOT_SOURCES}
		${IMGUI_IMTOOLS_SOURCES}
		${IMGUI_IMGUIZMO_SOURCES}
		${IMGUI_IMCOOLBAR_SOURCES}
		${IMGUI_IMGUIPACK_SOURCES}
		${IMGUI_IMGUIMARKDONW_SOURCES}
		${IMGUI_IMGRADIENTHDR_SOURCES}
		${IMGUI_IMGUIFILEDIALOG_SOURCES}
		${IMGUI_IMGUINODEEDITOR_SOURCES}
		${IMGUI_INAPPGPUPROFILER_SOURCES}
		${IMGUI_IMGUICOLORTEXTEDIT_SOURCES}
	)
	target_compile_definitions(${PROJECT} INTERFACE IMGUI_PACK_SHARED_LIBS)
	set_target_properties(${PROJECT} PROPERTIES POSITION_INDEPENDENT_CODE ON)
else()
	set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
	add_library(${PROJECT} STATIC 
		${IMGUI_SOURCES} 
		${IMGUI_IMPLOT_SOURCES}
		${IMGUI_IMTOOLS_SOURCES}
		${IMGUI_IMGUIZMO_SOURCES}
		${IMGUI_IMCOOLBAR_SOURCES}
		${IMGUI_IMGUIPACK_SOURCES}
		${IMGUI_IMGUIMARKDONW_SOURCES}
		${IMGUI_IMGRADIENTHDR_SOURCES}
		${IMGUI_IMGUIFILEDIALOG_SOURCES}
		${IMGUI_IMGUINODEEDITOR_SOURCES}
		${IMGUI_INAPPGPUPROFILER_SOURCES}
		${IMGUI_IMGUICOLORTEXTEDIT_SOURCES}
	)
endif()

set(IMGUIPACK_INCLUDE_DIRS
	${CTOOLS_INCLUDE_DIR}
	${TINYXML2_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_markdown/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/inappgpuprofiler/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguicolottextedit/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/Source
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/Include)

target_include_directories(${PROJECT} PRIVATE
	${IMGUIPACK_INCLUDE_DIRS}
)

if(UNIX)
    target_compile_options(${PROJECT} PUBLIC "-Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter")
endif()
 
if(USE_DEBUG_SANITIZER)
	target_compile_options(${PROJECT} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -static-libasan -static-libasan>)
	target_link_options(${PROJECT} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -static-libasan>)
	message("Address Sanitizer enabled for projet ${PROJECT}")
endif()

target_link_libraries(${PROJECT}
	${CTOOLS_LIBRARIES}
	${TINYXML2_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

set(IMGUIPACK_INCLUDE_DIRS ${IMGUIPACK_INCLUDE_DIRS} PARENT_SCOPE)
set(IMGUIPACK_LIBRARIES ${PROJECT} PARENT_SCOPE)

#install(FILES 
#    inc/someHeader.h
#    inc/someHeaderB.h
#    DESTINATION include/MyLibrary
#)