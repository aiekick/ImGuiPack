option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
option(GLFW_INSTALL OFF)
add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/glfw)

set_target_properties(glfw PROPERTIES FOLDER 3rdparty)
set_target_properties(update_mappings PROPERTIES FOLDER 3rdparty)

set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/glfw/include)
set(GLFW_DEFINITIONS -DGLFW_INCLUDE_NONE)
set(GLFW_LIBRARIES ${GLFW_LIBRARIES} glfw)
