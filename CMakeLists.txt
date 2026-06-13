cmake_minimum_required(VERSION 3.20)
project(AHCPCEMU CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Include)

# 1. Compile Emulator Module Logic
add_library(CPCEMUCORE STATIC
    Source/cpcbus.cpp
    Source/cpccpu.cpp
    Source/cpcdsk.cpp
    Source/cpcemu.cpp
)

# 2. Compile Native Window App Driver Target
add_executable(CPCEMU 
    Source/main.cpp
    Source/cpcapp.cpp
)

find_package(SDL3 REQUIRED)
target_link_libraries(CPCEMUCORE PRIVATE SDL3::SDL3)
target_link_libraries(CPCEMU PRIVATE CPCEMUCORE SDL3::SDL3)