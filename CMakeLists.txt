cmake_minimum_required(VERSION 3.20)

set(PROJECT ImGuiPack)

enable_language(C CXX)

project(
	${PROJECT} 
	LANGUAGES CXX
)

option(USE_IMPLOT "Use ImPlot" OFF)
option(USE_IM_TOOLS "Use ImTools" OFF)
option(USE_IM_NODES "Use ImNodes" OFF)
option(USE_IM_GUIZMO "Use ImGuizmo" OFF)
option(USE_IM_COOL_BAR "Use ImCoolBar" OFF)
option(USE_IMGUI_MARKDOW "Use ImGuiMarkDown" OFF)
option(USE_IM_GRADIENT_HDR "Use ImGradientHDR" OFF)
option(USE_IMGUI_NODE_FLOW "Use ImGuiNodeFlow" OFF)
option(USE_IMGUI_NODE_EDITOR "ImGuiNodeEditor" OFF)
option(USE_IMGUI_FILE_DIALOG "Use ImGuiFileDialog" OFF)
option(USE_IMGUI_COLOR_TEXT_EDIT "Use ImGuiColorTextEdit" OFF)
option(IMGUIPACK_USE_STD_FILESYSTEM "Enable the use of std::filesystem in ImGuiFileDialog" OFF)

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
  set(CMAKE_DEBUG_POSTFIX _debug)
endif()
if(NOT CMAKE_RELEASE_POSTFIX)
  set(CMAKE_RELEASE_POSTFIX)
endif()
if(NOT CMAKE_MINSIZEREL_POSTFIX)
  set(CMAKE_MINSIZEREL_POSTFIX _minsizerel)
endif()
if(NOT CMAKE_RELWITHDEBINFO_POSTFIX)
  set(CMAKE_RELWITHDEBINFO_POSTFIX _reldeb)
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
	set_target_properties(freetype PROPERTIES FOLDER Libs/Shared)
else()
	set_target_properties(freetype PROPERTIES FOLDER Libs/Static)
endif()

#####################################################
##### REGEX for ImGuiColorTextEdit
#####################################################

if (USE_IMGUI_COLOR_TEXT_EDIT)
	message("ImGuiColorTextEdit is using the library boost_regex")
	set(BOOST_REGEX_LIBRARIES boost_regex)
	set(BOOST_REGEX_STANDALONE ON CACHE BOOL "" FORCE)
	set(BOOST_REGEX_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/regex/include)
	add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/regex EXCLUDE_FROM_ALL)
	if(USE_SHARED_LIBS)
		set_target_properties(boost_regex PROPERTIES FOLDER libs/Shared)
	else()
		set_target_properties(boost_regex PROPERTIES FOLDER libs/Static)
	endif()
endif()

#####################################################
#####################################################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

add_definitions(-DUSE_STD_FILESYSTEM)
add_definitions(-DFT_DEBUG_LEVEL_TRACE)
add_definitions(-DIMGUI_ENABLE_FREETYPE)
add_definitions(-DIMGUI_DEFINE_MATH_OPERATORS)
#add_definitions(-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)
#add_definitions(-DIMGUI_USER_CONFIG="${CMAKE_CURRENT_SOURCE_DIR}/CustomImConfig.h")

#####################################################
##### IMGUI
#####################################################

file(GLOB IMGUI_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_docking/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_docking/*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_docking/misc/freetype/imgui_freetype.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_docking/misc/freetype/imgui_freetype.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_SOURCES})

#####################################################
##### IMPLOT
#####################################################

if (USE_IMPLOT)
	message("USE IM PLOT")
	file(GLOB IMGUI_IMPLOT_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMPLOT_SOURCES})
endif()

#####################################################
##### IMCOOLBAR
#####################################################

if (USE_IM_COOL_BAR)
	message("USE IM COOLBAR")
	file(GLOB IMGUI_IMCOOLBAR_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMCOOLBAR_SOURCES})
endif()

#####################################################
##### IMGRADIENTHDR
#####################################################

if (USE_IM_GRADIENT_HDR)
	message("USE GRADIENT HDR")
	file(GLOB IMGUI_IMGRADIENTHDR_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/src/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/src/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGRADIENTHDR_SOURCES})
endif()

#####################################################
##### IMGUIFILEDIALOG
#####################################################

if (USE_IMGUI_FILE_DIALOG)
	message("USE IMGUI FILE DIALOG")
	file(GLOB IMGUI_IMGUIFILEDIALOG_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIFILEDIALOG_SOURCES})
endif()

#####################################################
##### IMGUIZMO
#####################################################

if (USE_IM_GUIZMO)
	message("USE IM GUIZMO")
	file(GLOB IMGUI_IMGUIZMO_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIZMO_SOURCES})
endif()

#####################################################
##### IMTOOLS
#####################################################

if (USE_IM_TOOLS)
	message("USE IN TOOLS")
	file(GLOB IMGUI_IMTOOLS_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMTOOLS_SOURCES})
endif()

#####################################################
##### IMGUINODEFLOW
#####################################################

if (USE_IMGUI_NODE_FLOW)
	message("USE IMGUI NODE FLOW")
	file(GLOB IMGUI_IMGUINODEFLOW_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodeflow/include/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodeflow/src/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodeflow/src/*.inl
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodeflow/src/*.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodeflow/*.md)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUINODEFLOW_SOURCES})
endif()

#####################################################
##### IMGUINODEEDITOR
#####################################################

if (USE_IMGUI_NODE_EDITOR)
	message("USE IMGUI NODE EDITOR")
	file(GLOB IMGUI_IMGUINODEEDITOR_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/*.inl
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUINODEEDITOR_SOURCES})
endif()

#####################################################
##### IMNODES
#####################################################

if (USE_IM_NODES)
	message("USE IM NODES")
	file(GLOB IMGUI_IMNODES_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodes/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodes/*.cpp
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodes/*.md)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMNODES_SOURCES})
endif()

#####################################################
##### IMGUIMARKDONW
#####################################################

if (USE_IMGUI_MARKDOW)
	message("USE IMGUI MARKDOWN")
	file(GLOB IMGUI_IMGUIMARKDONW_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_markdown/*.h)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIMARKDONW_SOURCES})
endif()

#####################################################
##### IMGUICOLORTEXTEDIT
#####################################################

if (USE_IMGUI_COLOR_TEXT_EDIT)
	message("USE IMGUI COLOR TEXT EDIT")
	file(GLOB IMGUI_IMGUICOLORTEXTEDIT_SOURCES
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguicolortextedit/*.h
			${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguicolortextedit/*.cpp)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUICOLORTEXTEDIT_SOURCES})
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
		${IMGUI_IMNODES_SOURCES}
		${IMGUI_IMTOOLS_SOURCES}
		${IMGUI_IMGUIZMO_SOURCES}
		${IMGUI_IMCOOLBAR_SOURCES}
		${IMGUI_IMGUIPACK_SOURCES}
		${IMGUI_IMGUIMARKDONW_SOURCES}
		${IMGUI_IMGRADIENTHDR_SOURCES}
		${IMGUI_IMGUINODEFLOW_SOURCES}
		${IMGUI_IMGUIFILEDIALOG_SOURCES}
		${IMGUI_IMGUINODEEDITOR_SOURCES}
		${IMGUI_IMGUICOLORTEXTEDIT_SOURCES}
	)
	target_compile_definitions(${PROJECT} INTERFACE BUILD_IMGUI_PACK_SHARED_LIBS)
	set_target_properties(${PROJECT} PROPERTIES POSITION_INDEPENDENT_CODE ON)
else()
	set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
	add_library(${PROJECT} STATIC 
		${IMGUI_SOURCES} 
		${IMGUI_IMPLOT_SOURCES}
		${IMGUI_IMNODES_SOURCES}
		${IMGUI_IMTOOLS_SOURCES}
		${IMGUI_IMGUIZMO_SOURCES}
		${IMGUI_IMCOOLBAR_SOURCES}
		${IMGUI_IMGUIPACK_SOURCES}
		${IMGUI_IMGUIMARKDONW_SOURCES}
		${IMGUI_IMGRADIENTHDR_SOURCES}
		${IMGUI_IMGUINODEFLOW_SOURCES}
		${IMGUI_IMGUIFILEDIALOG_SOURCES}
		${IMGUI_IMGUINODEEDITOR_SOURCES}
		${IMGUI_IMGUICOLORTEXTEDIT_SOURCES}
	)
endif()

set(IMGUIPACK_INCLUDE_DIRS
	${CTOOLS_INCLUDE_DIR}
	${TINYXML2_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIR}
	${BOOST_REGEX_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodes/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_docking/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_markdown/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguicolortextedit/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imnodeflow/include/
)

message("IMGUIPACK CTOOLS_INCLUDE_DIR : ${CTOOLS_INCLUDE_DIR}")
message("IMGUIPACK TINYXML2_INCLUDE_DIR : ${TINYXML2_INCLUDE_DIR}")
target_include_directories(${PROJECT} PUBLIC
	${BOOST_REGEX_INCLUDE_DIR}
	${IMGUIPACK_INCLUDE_DIRS}
	${TINYXML2_INCLUDE_DIR}
	${CTOOLS_INCLUDE_DIR}
)

if(UNIX)
    target_compile_options(${PROJECT} PUBLIC "-Wno-unknown-pragmas -Wno-unused-variable -Wno-unused-parameter")
endif()

if (USE_IMPLOT)
	target_compile_definitions(${PROJECT} PUBLIC USE_IMPLOT)
endif()
if (USE_IM_COOL_BAR)
	target_compile_definitions(${PROJECT} PUBLIC USE_IM_COOL_BAR)
endif()
if (USE_IM_GRADIENT_HDR)
	target_compile_definitions(${PROJECT} PUBLIC USE_IM_GRADIENT_HDR)
endif()
if (USE_IMGUI_FILE_DIALOG)
	target_compile_definitions(${PROJECT} PUBLIC USE_IMGUI_FILE_DIALOG)
endif()
if (USE_IM_GUIZMO)
	target_compile_definitions(${PROJECT} PUBLIC USE_IM_GUIZMO)
endif()
if (USE_IM_TOOLS)
	target_compile_definitions(${PROJECT} PUBLIC USE_IM_TOOLS)
endif()
if (USE_IM_NODES)
	target_compile_definitions(${PROJECT} PUBLIC USE_IM_NODES)
endif()
if (USE_IMGUI_NODE_FLOW)
	target_compile_definitions(${PROJECT} PUBLIC USE_IMGUI_NODE_FLOW)
endif()
if (USE_IMGUI_NODE_EDITOR)
	target_compile_definitions(${PROJECT} PUBLIC USE_IMGUI_NODE_EDITOR)
endif()
if (USE_IMGUI_MARKDOW)
	target_compile_definitions(${PROJECT} PUBLIC USE_IMGUI_MARKDOW)
endif()
if (USE_IMGUI_COLOR_TEXT_EDIT)
	target_compile_definitions(${PROJECT} PUBLIC USE_IMGUI_COLOR_TEXT_EDIT)
endif()
 
if(USE_DEBUG_SANITIZER)
	target_compile_options(${PROJECT} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -static-libasan -static-libasan>)
	target_link_options(${PROJECT} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address -static-libasan>)
	message("Address Sanitizer enabled for projet ${PROJECT}")
endif()

target_link_libraries(${PROJECT}
	${BOOST_REGEX_LIBRARIES}
	${TINYXML2_LIBRARIES}
	${FREETYPE_LIBRARIES}
	${CTOOLS_LIBRARIES}
)

set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "imguipack")

set(IMGUIPACK_INCLUDE_DIRS ${IMGUIPACK_INCLUDE_DIRS} PARENT_SCOPE)
set(IMGUIPACK_LIBRARIES ${PROJECT} PARENT_SCOPE)
set(FREETYPE_LIBRARIES ${FREETYPE_LIBRARIES} PARENT_SCOPE)

install(TARGETS ${PROJECT} RUNTIME DESTINATION / COMPONENT APP_LIBS_${TINYXML2_LIBRARIES})
install(TARGETS ${TINYXML2_LIBRARIES} RUNTIME DESTINATION / COMPONENT APP_LIBS_${TINYXML2_LIBRARIES})
install(TARGETS ${FREETYPE_LIBRARIES} RUNTIME DESTINATION / COMPONENT APP_LIBS_${FREETYPE_LIBRARIES})
