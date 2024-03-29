cmake_minimum_required(VERSION 3.15)
project(ace VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

set(EXE_NAME main)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.h)

set(TF_BUILD_TESTS OFF)
set(TF_BUILD_EXAMPLES OFF)

set(ENKITS_BUILD_EXAMPLES OFF)
set(ENKITS_BUILD_C_INTERFACE OFF)

add_executable(${EXE_NAME} ${SOURCE_FILES})

# raylib
include(cmake/raylib.cmake)

add_subdirectory(external/spdlog)
add_subdirectory(external/taskflow)
add_subdirectory(external/argparse)

# raygui
add_compile_definitions(RAYGUI_IMPLEMENTATION TRUE)
add_subdirectory(external/raygui/projects/CMake)


# stb
target_include_directories(${EXE_NAME} PRIVATE external/stb)

target_link_libraries(${EXE_NAME} spdlog)
target_link_libraries(${EXE_NAME} Taskflow)
target_link_libraries(${EXE_NAME} raylib)
target_link_libraries(${EXE_NAME} argparse)
target_link_libraries(${EXE_NAME} raygui)

if (APPLE)
    include(cmake/macos.cmake)
endif()
