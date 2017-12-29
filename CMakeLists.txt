#the main CMakeLists.txt file
cmake_minimum_required(VERSION 2.8)
project(cpp_spec)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)
#collect source files
#file(GLOB_RECURSE SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp")
file(GLOB SRC_FILES "*.cpp")

message(STATUS "in top dir : source files => ${SRC_FILES}")

foreach(FILE ${SRC_FILES})
    message(STATUS "in top dir : file=> ${FILE}")
    string(REGEX REPLACE ".+/(.+)\\.cpp$" "\\1" PROJ ${FILE})
    add_executable(${PROJ} ${FILE})
    message(STATUS "in top dir : name => ${PROJ}")
endforeach()