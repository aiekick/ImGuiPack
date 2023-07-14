cmake_minimum_required(VERSION 3.1)

set(PROJECT ImGuiPackApp)

enable_language(C CXX)
project(${PROJECT} CXX)

include(cmake/3rdparty.cmake)

if(MSVC)
    add_definitions(-DMSVC)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Wdeprecated-declarations -Wunused-parameter")
endif ()

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)

if (USE_NEW_IMGUI_KEY_SYSTEM)
	add_definitions(-DIMGUI_DISABLE_OBSOLETE_KEYIO)
endif()

file(GLOB MAIN_SOURCES ${CMAKE_SOURCE_DIR}/main.cpp)
source_group(main FILES ${IMGUI_IMPL_SRC})

file(GLOB_RECURSE SRC_SOURCES ${CMAKE_SOURCE_DIR}/src/*.*)
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX src FILES ${SRC_SOURCES})

file(GLOB IMGUIIMPL_SOURCES 
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.cpp
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_glfw.h
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.cpp
		${CMAKE_SOURCE_DIR}/3rdparty/imgui/backends/imgui_impl_opengl3.h)
source_group(src\\ImGuiImpl FILES ${IMGUIIMPL_SOURCES})

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
	${IMGUIIMPL_SOURCES}
)

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

