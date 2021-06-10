project(opengl)

file(GLOB_RECURSE SRCS
		"${PROJECT_SOURCE_DIR}/*.cpp"
		"${PROJECT_SOURCE_DIR}/*.h"
		"${PROJECT_SOURCE_DIR}/*.hpp"
		"${PROJECT_SOURCE_DIR}/*.vert"
		"${PROJECT_SOURCE_DIR}/*.frag")
		
add_library(opengl STATIC ${SRCS})

FOREACH(_SRC IN ITEMS ${SRCS})
    GET_FILENAME_COMPONENT(SRC "${_SRC}" PATH)
    STRING(REPLACE "${PROJECT_SOURCE_DIR}/include" "include" _GRP_PATH "${SRC}")
    STRING(REPLACE "${PROJECT_SOURCE_DIR}/src" "src" _GRP_PATH "${_GRP_PATH}")
	STRING(REPLACE "${PROJECT_SOURCE_DIR}/shaders" "shaders" _GRP_PATH "${_GRP_PATH}")
    STRING(REPLACE "/" "\\" _GRP_PATH "${_GRP_PATH}")
    SOURCE_GROUP("${_GRP_PATH}" FILES "${_SRC}")
ENDFOREACH()

target_include_directories(opengl PUBLIC "${PROJECT_SOURCE_DIR}/include/")
target_link_libraries(opengl PUBLIC assimp glfw glad imgui glm stb_image)