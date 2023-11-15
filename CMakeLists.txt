cmake_minimum_required(VERSION 3.0)
project(VisiumEditor)

# Find the required packages
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(TGUI 0.9 REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add all source files from the src directory
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# Create the executable from the source files
add_executable(VisiumEditor ${SOURCES})

# Link libraries to the executable
target_link_libraries(VisiumEditor tgui sfml-graphics sfml-window sfml-system)
