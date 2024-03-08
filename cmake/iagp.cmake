set(IAGP_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/InAppGpuProfiler)
set(IAGP_LIBRARIES InAppGpuProfiler)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/InAppGpuProfiler)

if(USE_SHARED_LIBS)
	set_target_properties(InAppGpuProfiler PROPERTIES FOLDER 3rdparty/Shared)
else()
	set_target_properties(InAppGpuProfiler PROPERTIES FOLDER 3rdparty/Static)
endif()

set_target_properties(InAppGpuProfiler PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}")
set_target_properties(InAppGpuProfiler PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(InAppGpuProfiler PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")


