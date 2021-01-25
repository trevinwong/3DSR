#pragma once
#include "shader.h"
#include "graphics.h"

class GouraudShader : public Shader
{
    public:
        GouraudShader(World& World, Frame& Frame) : 
            Shader(World, Frame)
        {
        }

        virtual ~GouraudShader() override
        {
        }

        vec4 vertex(Vertex& vertex, mat4& model) override
        {
            intensity = std::max(dot(vertex.normal, world.get_light()), 0.1f);
            // TODO: pass in t,b,l,r,n,f for perspective
            // TODO: do actual clipping?
            vec4 model_coords = model * vertex.position;
            vec4 clip_coords = perspective() * lookAt(world.get_eye(), world.get_look_at_pt()) * model_coords;
            vec4 ndcs = clip_coords / clip_coords.w; 
            vec4 viewport_coords = viewport(frame) * ndcs;
            viewport_coords.x = (int) viewport_coords.x; // Convert to int to avoid black gaps between triangles.
            viewport_coords.y = (int) viewport_coords.y; // Convert to int to avoid black gaps between triangles.
            viewport_coords.w = clip_coords.w; // Keep wn (aka -z) for perspective-correct linear interpolation.
            return viewport_coords;
        }

        bool fragment(float& color) override
        {
            return false;
        }
    private:
        float intensity;
};