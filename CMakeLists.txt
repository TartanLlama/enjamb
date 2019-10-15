# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.5)

project ("enjamb")

find_package(ICU COMPONENTS uc REQUIRED)

# Include sub-projects.
add_subdirectory ("enjamb")
