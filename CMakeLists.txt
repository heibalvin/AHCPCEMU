cmake_minimum_required(VERSION 3.22)
project(AHCPCPEMU VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL3 REQUIRED)

option(AHC_DEV_MODE "Enable development console mode on macOS" ON)

# ------------------------------------------------------------------------------
# 1. SHARED CORE ENGINE MODULE (Renamed to CPCEMUCORE)
# ------------------------------------------------------------------------------
set(CORE_SOURCES
    Source/cpcemu.cpp
    Source/cpclog.cpp   # Lowercase
)
set(CORE_HEADERS
    Include/cpcemu.h
    Include/cpclog.h    # Lowercase
)

add_library(CPCEMUCORE STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(CPCEMUCORE PUBLIC Include)
target_link_libraries(CPCEMUCORE PUBLIC SDL3::SDL3)

# ------------------------------------------------------------------------------
# 2. TARGET A: GRAPHICAL APPLICATION (main_app.cpp)
# ------------------------------------------------------------------------------
set(GUI_SOURCES
    Source/main_app.cpp
    Source/cpcapp.cpp
)
set(GUI_HEADERS
    Include/cpcapp.h
)

set(GUI_TARGET_NAME "AHCPCPEMUAPP")

if(APPLE AND NOT AHC_DEV_MODE)
    add_executable(${GUI_TARGET_NAME} MACOSX_BUNDLE ${GUI_SOURCES} ${GUI_HEADERS})
    
    set_target_properties(${GUI_TARGET_NAME} PROPERTIES
        MACOSX_BUNDLE_BUNDLE_NAME "AHCPCPEMUAPP"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.akashatek.ahcepcpemu"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
    )

    file(GLOB ROM_RESOURCES "Resources/*")
    target_sources(${GUI_TARGET_NAME} PRIVATE ${ROM_RESOURCES})
    set_source_files_properties(${ROM_RESOURCES} PROPERTIES MACOSX_BUNDLE_RESOURCE TRUE)
else()
    add_executable(${GUI_TARGET_NAME} ${GUI_SOURCES} ${GUI_HEADERS})
    
    add_custom_command(TARGET ${GUI_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
        "$<TARGET_FILE_DIR:${GUI_TARGET_NAME}>/Resources"
    )
endif()

target_include_directories(${GUI_TARGET_NAME} PRIVATE Include)
target_link_libraries(${GUI_TARGET_NAME} PRIVATE CPCEMUCORE)

# ------------------------------------------------------------------------------
# 3. TARGET B: HEADLESS AUTOMATED TERMINAL RUNNER (main.cpp)
# ------------------------------------------------------------------------------
set(HEADLESS_TARGET_NAME "AHCPCPEMU")

add_executable(${HEADLESS_TARGET_NAME} Source/main.cpp)
target_include_directories(${HEADLESS_TARGET_NAME} PRIVATE Include)
target_link_libraries(${HEADLESS_TARGET_NAME} PRIVATE CPCEMUCORE)

# Sync assets next to the headless target binary
add_custom_command(TARGET ${HEADLESS_TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/Resources"
    "$<TARGET_FILE_DIR:${HEADLESS_TARGET_NAME}>/Resources"
)