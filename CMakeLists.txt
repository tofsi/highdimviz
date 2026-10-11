cmake_minimum_required(VERSION 3.24)

project(highdimviz LANGUAGES C CXX)

include(FetchContent)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    csv_parser
    GIT_REPOSITORY https://github.com/vincentlaucsb/csv-parser.git
    GIT_TAG v5.4.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(csv_parser)

add_library(glad STATIC external/glad/src/gl.c)
target_include_directories(glad PUBLIC external/glad/include)

add_executable(highdimviz
    src/main.cpp
    src/Camera.cpp
    src/ShaderProgram.cpp
    src/SphereRenderer.cpp
    src/Projection.cpp
    src/SceneFit.cpp
    src/Dataset.cpp
)
target_compile_features(highdimviz PRIVATE cxx_std_20)
set_target_properties(highdimviz PROPERTIES CXX_EXTENSIONS OFF)
target_compile_definitions(highdimviz PRIVATE GLFW_INCLUDE_NONE)
target_link_libraries(highdimviz PRIVATE glfw glm glad csv)

if(MSVC)
    target_compile_options(highdimviz PRIVATE /W4 /permissive-)
else()
    target_compile_options(highdimviz PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(camera_tests tests/CameraTests.cpp src/Camera.cpp)
    target_include_directories(camera_tests PRIVATE src)
    target_compile_features(camera_tests PRIVATE cxx_std_20)
    target_compile_definitions(camera_tests PRIVATE GLFW_INCLUDE_NONE)
    target_link_libraries(camera_tests PRIVATE glfw glm)
    add_test(NAME camera_math COMMAND camera_tests)
    add_test(NAME rendering_smoke COMMAND highdimviz --smoke-test)
    set_tests_properties(rendering_smoke PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
endif()
