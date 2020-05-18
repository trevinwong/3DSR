#pragma once
#include <vector>
#include "world.h"
#include "texture.h"
#include "frame.h"
#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"

class Rasterizer
{
    public:
        Rasterizer() = default;
        Rasterizer(World& w, Frame& f);
        // Render a 2D image of the world from the eye's view onto the frame.
        void render();

    private:
        void setup_vertices();

        void draw_line(int x0, int y0, int x1, int y1, Frame& frame);
        void draw_line(vec3& v0, vec3& v1, vec3& v2, Frame& frame);

        void draw_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, Texture* texture);

        mat4 lookAt(vec3 eye, vec3 target, vec3 up = vec3(0, 1, 0));
        mat4 get_perspective_matrix();
        mat4 get_viewport_matrix();

        void setup_zbuffer();
        
        World& world;
        Frame& frame;
        float* z_buffer = nullptr;
};
