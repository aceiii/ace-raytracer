cmake_minimum_required(VERSION 3.15)
project(ace VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(EXE_NAME main)

set(SOURCE_FILES
    main.cpp
    colour.cpp
    sphere.cpp
    hittable_list.cpp
    raylib_window.cpp
    # stb_impl.cpp
)

set(TF_BUILD_TESTS OFF)
set(TF_BUILD_EXAMPLES OFF)

set(ENKITS_BUILD_EXAMPLES OFF)
set(ENKITS_BUILD_C_INTERFACE OFF)

add_executable(${EXE_NAME} ${SOURCE_FILES})

add_subdirectory(external/spdlog)
add_subdirectory(external/taskflow)
add_subdirectory(external/raylib)
add_subdirectory(external/argparse)

# stb
target_include_directories(${EXE_NAME} PRIVATE external/stb)

target_link_libraries(${EXE_NAME} spdlog)
target_link_libraries(${EXE_NAME} Taskflow)
target_link_libraries(${EXE_NAME} raylib)
target_link_libraries(${EXE_NAME} argparse)

if (APPLE)
    include(cmake/macos.cmake)
endif()
