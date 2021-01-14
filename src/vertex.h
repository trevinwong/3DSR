#pragma once
#include "vec4.h"
#include "vec3.h"
#include "vec2.h"
#include "texture.h"
#include <vector>

// TODO: come up with some "vertex" class that better represents pure data parsed from the .obj file
// TODO: come up with some "vertex" class that better represents varying variables at a vertex not just the pure data
class Vertex
{
    public:
        vec4 position;
        vec4 normal;
        vec2 uv; 
        vec3 world_coords;
        float intensity;
};
