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
        void render();

    private:
		void draw_triangle(std::vector<vec4> coords);
        void draw_wireframe_triangle(std::vector<vec4> coords);
        void draw_line(int x0, int y0, int x1, int y1);
        void setup_zbuffer();
        
        World& world;
        Frame& frame;
        Shader& shader;
        float* z_buffer = nullptr; // TODO: deallocate z_buffer
};
