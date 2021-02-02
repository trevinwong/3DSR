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
  src/mat4.cpp
  src/frame.cpp
  src/object.cpp
  src/texture.cpp
  src/mesh.cpp
  src/world.cpp
  src/renderer.cpp
  src/color.h
  src/face.h
  src/frame.h
  src/graphics.h
  src/mat4.h
  src/mesh.h
  src/object.h
  src/renderer.h
  src/texture.h
  src/utils.h
  src/vec2.h
  src/vec3.h
  src/vec4.h
  src/vertex.h
  src/world.h
  src/shaders/gouraud_shader.h
  src/shaders/phong_shader.h
  src/shaders/shader.h
)

# Find the OS.
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(IS_OS_LINUX 1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(IS_OS_WINDOWS 1)
else()
  message(FATAL_ERROR "OS ${CMAKE_SYSTEM_NAME} was not recognized")
endif()


# Do not generate ZERO_CHECK
set(CMAKE_SUPPRESS_REGENERATION true)

# Add the executable, which should be built from the given source (.cpp) files.
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Directories to look in for includes when building the executable.
target_include_directories(${PROJECT_NAME} PUBLIC src/)
target_include_directories(${PROJECT_NAME} PUBLIC ext/)

# Added this so policy CMP0065 doesn't scream
set_target_properties(${PROJECT_NAME} PROPERTIES ENABLE_EXPORTS 0)

# Link in SDL2.
if (IS_OS_LINUX)
    # If we're on Linux, we can just use PkgConfig to find and link in SDL2
    find_package(PkgConfig REQUIRED)
    pkg_search_module(SDL2 REQURIED sdl2)
elseif (IS_OS_WINDOWS)
    # Allows us to build a Windows app in Visual Studio but still keep int main() as the entry point
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
    set(SDL2_FOUND TRUE)
    set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/include/SDL")
    # Check if x64 or x86
    if (${CMAKE_SIZEOF_VOID_P} MATCHES "8")
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.lib")
	set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x64.dll")
    else()
        set(SDL2_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.lib")
	set(SDL_DLL "${CMAKE_CURRENT_SOURCE_DIR}/ext/sdl/lib/SDL2-x86.dll")
    endif()

    # Copy and rename .dll's
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL_DLL}"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/SDL2.dll")

    add_definitions("-DNOMINMAX")
    add_definitions("-DWIN32_LEAN_AND_MEAN")
    add_definitions("-D_SCL_SECURE_NO_WARNINGS")
    add_definitions("-D_CRT_SECURE_NO_WARNINGS")
endif()

if (NOT SDL2_FOUND)
  message(FATAL_ERROR "Can't find SDL2." )
endif()

# Copies over our /obj directory where the project is actually built so it can find the appropriate paths.
# Inspired by JamesGriffin/3D-Rasterizer.
file(COPY obj/ DESTINATION "${PROJECT_BINARY_DIR}/obj")

# Copies over our /img directory which contains all of our textures.
file(COPY img/ DESTINATION "${PROJECT_BINARY_DIR}/img")

target_include_directories(${PROJECT_NAME} PUBLIC ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SDL2_LIBRARIES})
