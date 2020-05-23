#pragma once
#include "vec4.h"
#include "vec3.h"
#include "vec2.h"
#include "texture.h"
#include <vector>

class Vertex
{
    public:
        vec4 position;
        vec4 normal;
        vec2 uv;
 
        // DEPRECATED: renamed to position
        vec3 local_coords;
        vec3 world_coords;
        // DEPRECATED: renamed to uv
        vec2 texture_coords;

        // DEPRECATED: vertex should be a pure abstraction of parsed data
        vec4 viewport_coords;
        vec4 clip_coords; 

        // DEPRECATED: should be passed to the fragment shader
        // For Gouraud shading.
        float intensity;
        // For Phong shading.
        vec3 perspective_correct_normal;
};
