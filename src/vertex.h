#pragma once
#include "vec4.h"
#include "vec3.h"
#include "vec2.h"
#include "texture.h"
#include <vector>

// A "vertex" is a point where two or more lines meet, but, in the context of this renderer, 
// it is also a unit of information containing all the information the rasterizer needs.
// TODO: I like "tinyrenderer's" abstraction of a shader. Look into trying to do something along the lines of that later. Not all vertex's should have the same attributes.
class Vertex
{
    public:
        vec3 local_coords;
        vec3 world_coords;
        vec3 normal;
        vec2 texture_coords;

        vec4 viewport_coords;
        vec4 clip_coords; 
        // For Gouraud shading.
        float intensity;
        // For Phong shading.
        vec3 perspective_correct_normal;
};
