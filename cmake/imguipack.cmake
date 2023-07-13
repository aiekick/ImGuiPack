set(IMGUIPACK_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack)

add_subdirectory(${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack)
    
set_target_properties(ImGuiPack PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(ImGuiPack PROPERTIES FOLDER 3rdparty)

set(IMGUIPACK_LIBRARIES ImGuiPack)

