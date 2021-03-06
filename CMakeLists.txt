#the main CMakeLists.txt file
cmake_minimum_required(VERSION 2.8)
project(cpp_spec)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS ON)

#
cmake_policy(SET CMP0037 NEW)

message(STATUS "in top dir : CMAKE_CXX_COMPILER_VERSION => ${CMAKE_CXX_COMPILER_VERSION}")

if(APPLE)
    message(STATUS "in top dir : the target system is an Apple platform ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++11 -fPIC -g")
elseif(WIN32)
    message(STATUS "in top dir : the target system is an Win32 platform")
elseif(UNIX)

else()
endif()

#show the compiler flags
message(STATUS "in top dir : CMAKE_CXX_FLAGS => ${CMAKE_CXX_FLAGS}")

#include directory
include_directories(include)

if(WIN32)
    include_directories(E:\\back\\repo\\boost_1_65_1\\)
	include_directories(E:\\back\\repo\\ACE_wrappers\\)	
    link_directories(E:\\back\\repo\\boost_1_65_1\\stage\\lib\\)
	link_directories(E:\\back\\repo\\ACE_wrappers\\lib\\)
elseif(APPLE)
    include_directories(/usr/local/include )
    link_directories(/usr/local/lib)
    include_directories(/Users/caoguanghui/Workspace/ACE_wrappers)
    link_directories(/Users/caoguanghui/Workspace/ACE_wrappers/lib)
endif()

set(CMAKE_EXE_LINKER_FLAGS_DEBUG   "${CMAKE_EXE_LINKER_FLAGS_DEBUG}   /DYNAMICBASE \"ACEd.lib\" /LIBPATH:${LIB_DEBUG}")

#collect source files
#file(GLOB_RECURSE SRC_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.cpp")

file(GLOB SRC_FILES "source/*.cpp")
#message(STATUS "in top dir : source files => ${SRC_FILES}")

foreach(FILE ${SRC_FILES})
    message(STATUS "in top dir : file=> ${FILE}")
    string(REGEX REPLACE ".+/(.+)\\.cpp$" "\\1" PROJ ${FILE})
    message(STATUS "in top dir : add exe name => ${PROJ}")
    if(PROJ STREQUAL "chapter_3")
        if(APPLE)
            message(STATUS "in top dir : ignore chapter_3")
        else()
        message(STATUS "in top dir : add chapter_3")
            add_executable(${PROJ} ${FILE})
        endif()
    else()
        add_executable(${PROJ} ${FILE})
    endif()    
endforeach()