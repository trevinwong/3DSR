# Support for Linux only right now.
cmake_minimum_required (VERSION 3.1)
project (3DSR)

# Set C++17.
if (POLICY CMP0025)
 cmake_policy(SET CMP0025 NEW)
endif ()
set (CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
  src/main.cpp
  src/frame.cpp
)

# Add the executable, which should be built from the given source (.cpp) files.
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Directories to look in for includes when building the executable.
target_include_directories(${PROJECT_NAME} PUBLIC src/)
target_include_directories(${PROJECT_NAME} PUBLIC lib/)

# Find SDL2 and link it in.
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2 REQURIED sdl2)

if (NOT SDL2_FOUND)
  message(FATAL_ERROR "Can't find SDL." )
endif()

# Copies over our /obj directory where the project is actually built so it can find the appropriate paths.
# Inspired by JamesGriffin/3D-Rasterizer.
file(COPY obj/ DESTINATION "${PROJECT_BINARY_DIR}/obj")

# Copies over our /img directory which contains all of our textures.
file(COPY img/ DESTINATION "${PROJECT_BINARY_DIR}/img")

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
