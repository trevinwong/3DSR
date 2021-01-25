#pragma once
#include "../world.h"
#include "../frame.h"
#include "../vec4.h"

class Shader
{
    public:
        Shader(World& World, Frame& Frame) :
            world(World), frame(Frame)
        {
        }

        virtual ~Shader()
        {
        }

        virtual vec4 vertex(Vertex& vertex, mat4& model)=0;
        virtual bool fragment(float& color)=0;
    protected:
        World& world;
        Frame& frame;
};