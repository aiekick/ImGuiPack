cmake_minimum_required(VERSION 3.1)

set(PROJECT ImGuiPack)
set(PROJECT_VER 1.89.6)

enable_language(C CXX)

project(
	${PROJECT} 
	VERSION ${PROJECT_VER} 
	LANGUAGES CXX)
	
option(IMGUIPACK_BUILD_SHARED_LIBS "Enable Shared Lib" OFF)
option(IMGUIPACK_USE_STD_FILESYSTEM "Enable the use of std::filesystem in ImGuiFileDialog" OFF)

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
set_target_properties(freetype PROPERTIES FOLDER 3rdparty)

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
add_definitions(-DIMGUI_DISABLE_OBSOLETE_FUNCTIONS)
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

file(GLOB IMGUI_IMPLOT_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_implot/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMPLOT_SOURCES})

#####################################################
##### IMCOOLBAR
#####################################################

file(GLOB IMGUI_IMCOOLBAR_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imcoolbar/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMCOOLBAR_SOURCES})

#####################################################
##### IMGRADIENTHDR
#####################################################

file(GLOB IMGUI_IMGRADIENTHDR_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/src/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/src/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGRADIENTHDR_SOURCES})

#####################################################
##### IMGUIFILEDIALOG
#####################################################

file(GLOB IMGUI_IMGUIFILEDIALOG_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIFILEDIALOG_SOURCES})

#####################################################
##### IMGUIZMO
#####################################################

file(GLOB IMGUI_IMGUIZMO_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguizmo/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIZMO_SOURCES})

#####################################################
##### IMTOOLS
#####################################################

file(GLOB IMGUI_IMTOOLS_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_tools/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMTOOLS_SOURCES})

#####################################################
##### IMGUINODEEDITOR
#####################################################

file(GLOB_RECURSE IMGUI_IMGUINODEEDITOR_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/*.h
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/*.cpp)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUINODEEDITOR_SOURCES})

#####################################################
##### IMGUIMARKDONW
#####################################################

file(GLOB IMGUI_IMGUIMARKDONW_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_markdown/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ PREFIX pack FILES ${IMGUI_IMGUIMARKDONW_SOURCES})

#####################################################
##### IMGUIPACK
#####################################################

file(GLOB IMGUI_IMGUIPACK_SOURCES
		${CMAKE_CURRENT_SOURCE_DIR}/*.h)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/ PREFIX header FILES ${IMGUI_IMGUIPACK_SOURCES})

#####################################################
#####################################################
#####################################################

if (IMGUIPACK_BUILD_SHARED_LIBS)
	set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
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
	)
	target_compile_definitions(${PROJECT} INTERFACE BUILD_SHARED_LIBS)
	set_target_properties(${PROJECT} PROPERTIES POSITION_INDEPENDENT_CODE ON)
else()
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
	)
endif()

message("IMGUIPACK -> CTOOLS_INCLUDE_DIR : ${CTOOLS_INCLUDE_DIR}")
message("IMGUIPACK -> TINYXML2_INCLUDE_DIR : ${TINYXML2_INCLUDE_DIR}")

target_include_directories(${PROJECT} PRIVATE
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
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/Include
)

message("IMGUIPACK -> CTOOLS_LIBRARIES : ${CTOOLS_LIBRARIES}")
message("IMGUIPACK -> TINYXML2_LIBRARIES : ${TINYXML2_LIBRARIES}")

target_link_libraries(${PROJECT}
	${CTOOLS_LIBRARIES}
	${TINYXML2_LIBRARIES}
	${FREETYPE_LIBRARIES}
)

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
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imgradienthdr/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_imguifiledialog/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/imgui_node_editor/NodeEditor/Include
PARENT_SCOPE)

set(IMGUIPACK_LIBRARIES ImGuiPack PARENT_SCOPE)

#install(FILES 
#    inc/someHeader.h
#    inc/someHeaderB.h
#    DESTINATION include/MyLibrary
#)