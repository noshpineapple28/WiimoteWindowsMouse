# 2009-2011 Ryan Pavlik <rpavlik@iastate.edu>
# http://academic.cleardefinition.com/
# Iowa State University HCI Graduate Program/VRAC

cmake_minimum_required(VERSION 3.0.0)

# Set package properties
project(WiimoteWindowsMouse
    VERSION 0.1.0
    DESCRIPTION "A program that can turn wiimotes into mice")

###
# Set up options
###
option(CPACK_MONOLITHIC_INSTALL "Only produce a single component installer, rather than multi-component." NO)

###
# Perform build configuration of dependencies
###

# Locally-developed modules dist'ed with this app - always have this first.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(NOT BUILD_SHARED_LIBS)
    add_definitions(-DWIIUSE_STATIC)
endif()

find_package(WinHID REQUIRED)
include_directories(${WINHID_INCLUDE_DIRS})
add_definitions(-D_WIN32_WINNT=0x0501)

###
# Build the project
###

# The lib is in the "src" subdirectory
add_subdirectory(lib)
add_subdirectory(src)

set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/src")