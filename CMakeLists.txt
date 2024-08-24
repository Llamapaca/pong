cmake_minimum_required(VERSION 3.4...3.28 FATAL_ERROR)

project(Pong CXX C)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(sources ${SOURCE_DIR}/pong/main.cpp)


add_executable(pong ${sources} ${includes})

add_subdirectory(${SOURCE_DIR}/engine)
target_link_libraries(pong PRIVATE engine)
add_dependencies(pong engine)

include(FetchContent)


FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

FetchContent_MakeAvailable(glfw)
set(GLFW_BUILD_EXAMPLES     OFF CACHE BOOL "Build Examples" FORCE)
set(GLFW_BUILD_TESTS        OFF CACHE BOOL "Build tests" FORCE)
set(GLFW_BUILD_DOCS         OFF CACHE BOOL "Build docs" FORCE)
set(GLFW_INSTALL            OFF CACHE BOOL "Configure an install" FORCE)
set(GLFW_BUILD_WAYLAND      ON  CACHE BOOL "Use Wayland" FORCE)
set(GLFW_BUILD_X11          OFF  CACHE BOOL "Don't Use X11" FORCE)

target_link_libraries(pong PRIVATE glfw)
add_dependencies(pong glfw)


FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.6
    SOURCE_SUBDIR cmake
)

FetchContent_MakeAvailable(glad)
glad_add_library(glad STATIC API gl:core=4.6)
target_link_libraries(pong PRIVATE glad)
add_dependencies(pong glad)


FetchContent_Declare(
	glm
	GIT_REPOSITORY https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)
set(GLM_TEST_ENABLE OFF CACHE BOOL "Disable Tests" FORCE)
set(GLM_ENABLE_CXX_20 ON CACHE BOOL "Enable C++ 20 Support" FORCE)
target_link_libraries(pong PRIVATE glm)
add_dependencies(pong glm)

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    target_include_directories(pong PRIVATE ${OPENGL_INCLUDE_DIRS})
    target_link_libraries(pong PRIVATE ${OPENGL_LIBRARIES})
else()
    message(FATAL_ERROR "OpenGL Could Not Be Found!")
endif()


set_target_properties(
    pong PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

set_target_properties(pong PROPERTIES FOLDER ${PROJECT_NAME})
