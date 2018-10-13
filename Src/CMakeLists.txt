# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.3)

# Include sub-projects.
add_subdirectory ("Server")
add_subdirectory("Client")

