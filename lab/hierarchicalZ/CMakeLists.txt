project(HierarchicalZ)

file(GLOB_RECURSE SRCS
		"${PROJECT_SOURCE_DIR}/*.cpp"
		"${PROJECT_SOURCE_DIR}/*.h"
		"${PROJECT_SOURCE_DIR}/shaders/*.*")
		
add_executable(HierarchicalZ ${SRCS})

target_include_directories(HierarchicalZ PUBLIC "${PROJECT_SOURCE_DIR}/include")
target_link_libraries(HierarchicalZ PUBLIC opengl)

set(test_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/HierarchicalZ/")
set_target_properties(HierarchicalZ
	PROPERTIES 
	RUNTIME_OUTPUT_DIRECTORY ${test_RUNTIME_OUTPUT_DIRECTORY}
)
# COPY_ASSIMP_BINARIES(${test_RUNTIME_OUTPUT_DIRECTORY})

FOREACH(_SRC IN ITEMS ${SRCS})
    GET_FILENAME_COMPONENT(SRC "${_SRC}" PATH)
    STRING(REPLACE "${PROJECT_SOURCE_DIR}/include" "include" _GRP_PATH "${SRC}")
    STRING(REPLACE "${PROJECT_SOURCE_DIR}/src" "src" _GRP_PATH "${_GRP_PATH}")
	STRING(REPLACE "${PROJECT_SOURCE_DIR}/shaders" "shaders" _GRP_PATH "${_GRP_PATH}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()

# set workDir
if(MSVC)
	set_target_properties(HierarchicalZ PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
endif()	