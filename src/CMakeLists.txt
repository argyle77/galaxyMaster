# Galaxy Emulation Project src/CMAKELists.txt 
# Created 2014_10_30 by Argyle

# Define the executable source - same as project name.
add_executable (${PROJECT_NAME} master.c)

# Configure the version header.
configure_file("version.h.in" "version.h")
add_library(display display.c)
add_library(init init.c)
add_library(patternSupport patternSupport.c)
add_library(pattern pattern.c)

# Included directories
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${PROJECT_BINARY_DIR}/bin)

# Linked Libraries
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} m SDL2_gfx init patternSupport display pattern)
