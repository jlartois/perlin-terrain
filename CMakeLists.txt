cmake_minimum_required(VERSION 3.0)

project(perlin-terrain)

set(CMAKE_CXX_STANDARD 11)

# Add OpenGL
find_package(OpenGL REQUIRED)

# Add Glad
add_library(glad src/glad.c)
include_directories(include)

# Add GLFW as a submodule
add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

# Add imgui
file(GLOB IMGUI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui/*.h )
source_group( "imgui" FILES ${IMGUI_SOURCES} )

# Your main executable
add_executable(${PROJECT_NAME} 
  src/main.cpp
  src/glHelper.h
  src/shader.h
  ${IMGUI_SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME} glfw glad ${CMAKE_DL_LIBS})

# make the current path available to the source code
target_compile_definitions(${PROJECT_NAME} PUBLIC CMAKELISTS_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")