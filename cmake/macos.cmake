
target_link_libraries(${EXE_NAME} "-framework IOKit")
target_link_libraries(${EXE_NAME} "-framework Cocoa")
target_link_libraries(${EXE_NAME} "-framework OpenGL")

if (TARGET_BUNDLE_NAME)
    add_executable(${TARGET_BUNDLE_NAME} MACOSX_BUNDLE ${SOURCE_FILES})

    set_target_properties(${TARGET_BUNDLE_NAME} PROPERTIES
        BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.borinouch.aceraytracer"
        MACOSX_BUNDLE_BUNDLE_NAME "Ace-Raytracer"
        MACOSX_BUNDLE_VERSION_NAME "0.0.1"
        MACOSX_BUNDLE_COPYRIGHT "Copyright Borin Ouch"
    )

    target_include_directories(${TARGET_BUNDLE_NAME} PRIVATE external/stb)

    target_link_libraries(${TARGET_BUNDLE_NAME} spdlog)
    target_link_libraries(${TARGET_BUNDLE_NAME} Taskflow)
    target_link_libraries(${TARGET_BUNDLE_NAME} raylib)
endif()
