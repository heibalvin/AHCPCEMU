cmake_minimum_required(VERSION 3.22)
project(AHCPCPEMU VERSION 1.0.0 LANGUAGES CXX)

# Force modern C++ parameters without platform extensions
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Locate official SDL3 build variables
find_package(SDL3 REQUIRED)

# Structural source and header aggregation
set(SOURCE_FILES
    Source/main.cpp
    Source/cpcapp.cpp
)

set(HEADER_FILES
    Include/cpcapp.h
)

# Detect if we are building for an Apple platform (macOS or iOS)
if(APPLE)
    # 1. Instruct CMake to compile a platform native GUI application bundle (.app)
    # instead of a raw command-line binary.
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES} ${HEADER_FILES})
    
    # 2. Define the application bundle properties required by Apple operating systems
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "AHCPCPEMU"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.akashatek.ahcepcpemu"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )

    # 3. Handle Resource deployment native to Apple Bundles
    # On macOS/iOS, assets belong inside the ".app/Contents/Resources" directory.
    file(GLOB ROM_RESOURCES "Resources/*")
    target_sources(${PROJECT_NAME} PRIVATE ${ROM_RESOURCES})
    set_source_files_properties(${ROM_RESOURCES} PROPERTIES
        MACOSX_BUNDLE_RESOURCE TRUE
    )
else()
    # Fallback standard compilation rule for Linux / Windows
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
    
    # Non-Apple deployment asset copy command
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources"
    )
endif()

# Inject path directives to locate header boundaries
target_include_directories(${PROJECT_NAME} PRIVATE Include)

# Bind native platform library dependencies (SDL3 handles Cocoa/Metal links under the hood)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)