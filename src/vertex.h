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
};
