cmake_minimum_required(VERSION 3.30)

set(PROJECT_NAME anvilruntime)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(BINARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUTPUT_DIRECTORY})

#DebugMode
set(CMAKE_BUILD_TYPE Release)

project(${PROJECT_NAME} CXX)

add_library(${PROJECT_NAME} STATIC
    src/anvilruntime.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE GL glfw GLU GLEW openal)

# Debug
project(${PROJECT_NAME}_test CXX)
add_executable(${PROJECT_NAME} tests/${PROJECT_NAME}.cpp)

target_link_directories(${PROJECT_NAME} PRIVATE bin)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE anvilruntime)

add_compile_options(-Wall -Wextra -Wpedantic -O3 -flto)
