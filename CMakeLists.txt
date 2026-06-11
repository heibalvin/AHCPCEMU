cmake_minimum_required(VERSION 3.22)
project(AHCPCPEMU VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)

# ------------------------------------------------------------------------------
# Development Switch: Set to ON to force console logging on macOS
# ------------------------------------------------------------------------------
option(AHC_DEV_MODE "Enable development console mode on macOS" ON)

set(SOURCE_FILES
    Source/main.cpp
    Source/cpcapp.cpp
)

set(HEADER_FILES
    Include/cpcapp.h
)

if(APPLE AND NOT AHC_DEV_MODE)
    # RETAIL MODE: Compiles a clean, standalone .app bundle package for users
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES} ${HEADER_FILES})
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "AHCPCPEMU"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.akashatek.ahcepcpemu"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )

    file(GLOB ROM_RESOURCES "Resources/*")
    target_sources(${PROJECT_NAME} PRIVATE ${ROM_RESOURCES})
    set_source_files_properties(${ROM_RESOURCES} PROPERTIES
        MACOSX_BUNDLE_RESOURCE TRUE
    )
else()
    # DEV MODE (macOS Console) or Standard Linux/Windows target compilation
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
    
    # Ensure assets are copied directly next to the loose command line binary
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
        COMMENT "Syncing emulator system ROM resource files to execution folder..."
    )
endif()

target_include_directories(${PROJECT_NAME} PRIVATE Include)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)