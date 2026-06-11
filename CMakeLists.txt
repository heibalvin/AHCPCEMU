cmake_minimum_required(VERSION 3.20)
project(AHCPCPEMU VERSION 1.0.0 LANGUAGES CXX)

# Force standard modern C++ compilation compliance rules
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and bind the cross-platform SDL3 development framework
find_package(SDL3 REQUIRED)

# 1. SHARED CORE ENGINE STATIC LIBRARY
set(CORE_SOURCES Source/cpcemu.cpp Source/cpclog.cpp Source/cpcdsk.cpp)
set(CORE_HEADERS Include/cpcemu.h Include/cpclog.h Include/cpccom.h Include/cpcdsk.h)

add_library(CPCEMUCORE STATIC ${CORE_SOURCES} ${CORE_HEADERS})
target_include_directories(CPCEMUCORE PUBLIC Include)
target_link_libraries(CPCEMUCORE PUBLIC SDL3::SDL3)

# 2. THE UNIQUE UNIFIED EXECUTABLE BINARY
add_executable(CPCEMU Source/main.cpp Source/cpcapp.cpp Include/cpcapp.h)
target_link_libraries(CPCEMU PRIVATE CPCEMUCORE SDL3::SDL3)