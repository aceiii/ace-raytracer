cmake_minimum_required(VERSION 3.15)
project(ace VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(EXE_NAME main)

set(SOURCE_FILES
    main.cpp
    stb_impl.cpp
)

# set(SPDLOG_FMT_EXTERNAL ON)
set(SPDLOG_FMT_EXTERNAL_HO ON)

set(TF_BUILD_TESTS OFF)
set(TF_BUILD_EXAMPLES OFF)

set(ENKITS_BUILD_EXAMPLES OFF)
set(ENKITS_BUILD_C_INTERFACE OFF)

add_executable(${EXE_NAME} ${SOURCE_FILES})

add_subdirectory(external/fmt)
add_subdirectory(external/spdlog)
add_subdirectory(external/taskflow)

# stb
target_include_directories(${EXE_NAME} PRIVATE external/stb)

target_link_libraries(${EXE_NAME} fmt)
target_link_libraries(${EXE_NAME} spdlog)
target_link_libraries(${EXE_NAME} Taskflow)

