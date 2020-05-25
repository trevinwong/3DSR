#pragma once
#include "vec4.h"
#include "vec3.h"
#include "vec2.h"
#include "texture.h"
#include <vector>

// TODO: come up with some vertex/shader abstraction for varying variables
class Vertex
{
    public:
        vec4 position;
        vec4 normal;
        vec2 uv; 
        vec3 world_coords;
        float intensity;
};
