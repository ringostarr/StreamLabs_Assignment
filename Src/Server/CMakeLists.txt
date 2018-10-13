# CMakeList.txt : CMake project for NamedPipesCpp, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set( SOURCE 
			server.cpp)
# Add source to this project's executable.
add_executable (Server "Server.cpp" "Server.h")

# TODO: Add tests and install targets if needed.
