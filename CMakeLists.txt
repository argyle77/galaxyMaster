# Galaxy Emulation Project CMAKELists.txt 
# Created 2014_10_30 by Argyle
cmake_minimum_required(VERSION 2.6)

# This can be read from ${PROJECT_NAME} after project() is called
project(galaxyEmulator)

# Let's set a version number.
set (EMULATOR_VERSION_MAJOR 0)
set (EMULATOR_VERSION_MINOR 4)
set (EMULATOR_VERSION_PATCH 0)
set (EMULATOR_VERSION_TWEAK 20141104)

message(STATUS "Configuring build version " ${EMULATOR_VERSION_MAJOR}.${EMULATOR_VERSION_MINOR}.${EMULATOR_VERSION_PATCH}.${EMULATOR_VERSION_TWEAK})

# gcc flags
add_compile_options(-Wall -g)
add_definitions(-DEMULATE)

# Libraries
include(FindPkgConfig)
PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
if ( NOT SDL2_FOUND )
  message ( FATAL_ERROR "SDL2 not found!" )
endif ( NOT SDL2_FOUND )

# Verbose output (or use: make VERBOSE=1)
#set ( CMAKE_VERBOSE_MAKEFILE on )

# Subdirectories
add_subdirectory (src bin)
