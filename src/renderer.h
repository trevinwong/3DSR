#pragma once
#include <vector>
#include "world.h"
#include "texture.h"
#include "frame.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "shaders/shader.h"

class Renderer
{
    public:
        Renderer() = default;
        Renderer(World& w, Frame& f, Shader& s);
        // Render a 2D image of the world from the eye's view onto the frame.
        void render();

    private:
        void draw_line(int x0, int y0, int x1, int y1, Frame& frame);
        void draw_line(vec3& v0, vec3& v1, vec3& v2, Frame& frame);

        void draw_triangle_new(std::vector<vec4> coords);
        void draw_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const std::shared_ptr<Texture>& texture);

        void setup_zbuffer();
        
        World& world;
        Frame& frame;
        Shader& shader;
        float* z_buffer = nullptr; // TODO: deallocate z_buffer
};
