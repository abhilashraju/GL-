cmake_minimum_required(VERSION 3.1)


project (GLplus)
find_package(OpenGL REQUIRED)
link_directories(${OPENGL_gl_LIBRARY})
# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()
# Add an executable
set(SOURCES
    # ${PROJECT_SOURCE_DIR}/examples/example1.cpp
    ${PROJECT_SOURCE_DIR}/examples/lighting.cpp
    ${PROJECT_SOURCE_DIR}/examples/glad.c
)
add_executable(GLplus ${SOURCES})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts" CACHE STRING "Set C++ Compiler Flags" FORCE)
set(CMAKE_CXX_STANDARD 17)
target_compile_features(GLplus PRIVATE cxx_std_17)
target_compile_features(GLplus PUBLIC cxx_auto_type)
target_include_directories(GLplus
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_SOURCE_DIR}/glad
        ${PROJECT_SOURCE_DIR}/KHR
        ${PROJECT_SOURCE_DIR}/
        ${PROJECT_SOURCE_DIR}/libs/glfw-3.3.4.bin.WIN64/include
        ${PROJECT_SOURCE_DIR}/thirdparty/glm/
)
target_link_libraries(GLplus glfw3 glfw3_mt)
target_link_libraries(GLplus ${OPENGL_gl_LIBRARY})
target_link_directories(GLplus
    PRIVATE 
    ${PROJECT_SOURCE_DIR}/libs/glfw-3.3.4.bin.WIN64/lib-vc2019
    )