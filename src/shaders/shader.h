#pragma once
#include "../world.h"
#include "../frame.h"
#include "../vec4.h"
#include "../vec3.h"

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

        virtual vec4 vertex(const Vertex& vertex, const mat4& model, int num_vert)=0;
        virtual bool fragment(const vec4& barycentric, uint32_t& color)=0;
		void set_texture(std::shared_ptr<Texture> Texture)
		{
			texture = Texture;
		}
    protected:
        World& world;
        Frame& frame;
		std::shared_ptr<Texture> texture;
};