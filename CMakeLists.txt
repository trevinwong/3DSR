# Support for Linux only right now.
cmake_minimum_required (VERSION 3.1)
project (3DSR)

# Set C++14.
if (POLICY CMP0025)
 cmake_policy(SET CMP0025 NEW)
endif ()
set (CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
  src/main.cpp
)

# Add the executable.
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

# Find SDL2 and link it in.
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQURIED sdl2)

if (NOT SDL2_FOUND)
  message(FATAL_ERROR "Can't find SDL." )
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
