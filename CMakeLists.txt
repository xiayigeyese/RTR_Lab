cmake_minimum_required (VERSION 3.5)

project(RTR_Lab)

set(CMAKE_CXX_STANDARD 17)

#====== import external lib ======
set(EXTERNAL_DIR "${PROJECT_SOURCE_DIR}/external")

# find assimp
set(ASSIMP_DIR "${EXTERNAL_DIR}/assimp-4.0/lib/cmake/assimp-4.0/") 
find_package(ASSIMP REQUIRED)
# add assimp, and call function to copy dll to target directory
add_library(assimp INTERFACE)
target_include_directories(assimp INTERFACE ${ASSIMP_INCLUDE_DIRS})
target_link_directories(assimp INTERFACE ${ASSIMP_LIBRARY_DIRS})
target_link_libraries(assimp INTERFACE ${ASSIMP_LIBRARIES})
if (WIN32)
	function(COPY_ASSIMP_BINARIES TargetDirectory)
	    file(COPY ${ASSIMP_ROOT_DIR}/bin/assimp-vc140-mt.dll DESTINATION ${TargetDirectory}/Debug)
		file(COPY ${ASSIMP_ROOT_DIR}/bin/assimp-vc140-mt.dll DESTINATION ${TargetDirectory}/Release)
	endfunction(COPY_ASSIMP_BINARIES)
endif(WIN32)

# add glfw
set(GLFW3_DIR "${EXTERNAL_DIR}/glfw/lib/cmake/glfw3/")
find_package(GLFW3 REQUIRED)

# add glad
add_library(glad "${EXTERNAL_DIR}/glad/src/glad.c")
target_include_directories(glad PUBLIC "${EXTERNAL_DIR}/glad/include/")

# add imgui --> imgui need glfw and glad
file(GLOB_RECURSE IMGUI_SRCS "${EXTERNAL_DIR}/imgui/src/*.cpp")
add_library(imgui ${IMGUI_SRCS})
target_include_directories(imgui PUBLIC "${EXTERNAL_DIR}/imgui/include")
target_link_libraries(imgui PUBLIC glfw)
target_link_libraries(imgui PUBLIC glad)

# add glm
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE "${EXTERNAL_DIR}/glm-0.9.9/include")

# add stb_image
add_library(stb_image INTERFACE)
target_include_directories(stb_image INTERFACE "${EXTERNAL_DIR}/stb_image")

#====== myopengl ======

add_subdirectory(opengl)

#====== test ======

add_subdirectory(lab)