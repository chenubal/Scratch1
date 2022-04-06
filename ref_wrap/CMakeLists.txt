# CMakeList.txt : CMake project for ref_wrap, include source and define
# project specific logic here.
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (ref_wrap "ref_wrap.cpp" "ref_wrap.h")

# TODO: Add tests and install targets if needed.
