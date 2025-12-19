cmake_minimum_required(VERSION 4.0.0)
project(project_maker CXX ASM_NASM)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    message("CCACHE FOUND")
endif()

include_directories( BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/Data_Structures/)
add_subdirectory(Test)
