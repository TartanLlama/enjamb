# CMakeList.txt : CMake project for enjamb, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.15)

# Add source to this project's executable.
add_executable (enjamb "enjamb.cpp")
target_link_libraries(enjamb ICU::uc)
set_property(TARGET enjamb PROPERTY CXX_STANDARD 17)

# TODO: Add tests and install targets if needed.
