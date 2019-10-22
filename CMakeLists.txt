cmake_minimum_required (VERSION 3.14)

project ("enjamb")

if(CMAKE_SYSTEM_NAME STREQUAL "Emscripten")
  message(Fetching ICU)
  # Building ICU for Emscripten is a *massive* pain, pull down binaries instead
  include(FetchContent)
  FetchContent_Declare(
    icu-emscripten
    GIT_REPOSITORY https://github.com/TartanLlama/icu-emscripten.git
    GIT_TAG        master
  )
  FetchContent_MakeAvailable(icu-emscripten)

  # Make sure CMake doesn't try to pull in system installations of ICU
  set(ICU_ROOT ${icu-emscripten_SOURCE_DIR})
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY NEVER)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE NEVER)
endif()

# We need the unicode and data components of ICU
find_package(ICU COMPONENTS uc dt REQUIRED)

add_subdirectory("lib")
add_subdirectory("tools")
