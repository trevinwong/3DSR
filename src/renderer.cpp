#include "renderer.h"
#include <limits>
#include <cmath>
#include <utility>
#include "utils.h"
#include "vertex.h"

Renderer::Renderer(World& w, Frame& f, Shader& s) :
    world(w), frame(f), shader(s)
{
}

void Renderer::render()
{
    frame.fill_frame_with_color(0xADD8E6);
    setup_zbuffer();

    for (Object* object : world.getObjects())
    {
        std::shared_ptr<Mesh>& mesh = object->getMesh();
        mat4 model = *object->getMat();
		shader.set_texture(mesh->getTexture());

        for (Face& face : mesh->getFaces())
        {
			int num_vert = 0;
            std::vector<vec4> coords;

            for (Vertex& vertex : face.vertices)
            {
                coords.push_back(shader.vertex(vertex, model, num_vert++));
            }

            // Foundations of 3D Computer Graphics, 12.2
            // Calculate the direction of the normal of the screen-space triangle, which is either towards -wn or +wn
            // If +wn: vertices are CCW, aka facing the front-side
            // If -wn: vertices are CW, aka facing the back-side
            float backface = ((coords[2].x - coords[1].x) * (coords[0].y - coords[1].y)) -
                             ((coords[2].y - coords[1].y) * (coords[0].x - coords[1].x));
            if (backface > 0) { draw_triangle(coords); }
        }
    }
}

void Renderer::draw_triangle(std::vector<vec4> coords)
{
	std::vector<vec2> screen_coords;
	screen_coords.push_back(vec2(coords[0]));
	screen_coords.push_back(vec2(coords[1]));
	screen_coords.push_back(vec2(coords[2]));

	vec2 edge0 = (screen_coords[1] - screen_coords[0]);
	vec2 edge1 = (screen_coords[2] - screen_coords[1]);
	vec2 edge2 = (screen_coords[0] - screen_coords[2]);

	int min_x = std::max(min3(screen_coords[0].x, screen_coords[1].x, screen_coords[2].x), 0);
	int max_x = std::min(max3(screen_coords[0].x, screen_coords[1].x, screen_coords[2].x), frame.w - 1);
	int min_y = std::max(min3(screen_coords[0].y, screen_coords[1].y, screen_coords[2].y), 0);
	int max_y = std::min(max3(screen_coords[0].y, screen_coords[1].y, screen_coords[2].y), frame.h - 1);

	for (int i = min_x; i <= max_x; i++)
	{
		for (int j = min_y; j <= max_y; j++)
		{
			vec2 point(i, j);
			vec2 v0_to_point = point - vec2(screen_coords[0]);
			vec2 v1_to_point = point - vec2(screen_coords[1]);
			vec2 v2_to_point = point - vec2(screen_coords[2]);

			float v0v1p = cross(edge0, v0_to_point);
			float v1v2p = cross(edge1, v1_to_point);
			float v2v0p = cross(edge2, v2_to_point);

			if (v0v1p >= 0 && v1v2p >= 0 && v2v0p >= 0)
			{
				float v0v1v2 = cross(screen_coords[1] - screen_coords[0], screen_coords[2] - screen_coords[0]);
				if (v0v1v2 == 0) return; // Discard degenerate triangles, whose area is 0.

				float b1 = v1v2p / v0v1v2;
				float b2 = v2v0p / v0v1v2;
				float b3 = v0v1p / v0v1v2;

				// Rational linear interpolation.
				// https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
				// Foundations of 3D Computer Graphics, Ch 13
				// Remember that we store the z-value inside of our w
				float wn_reciprocal = (b1 * (1.0f / coords[0].w)) + (b2 * (1.0f / coords[1].w)) + (b3 * (1.0f / coords[2].w));
				float wn = (1.0f / wn_reciprocal);
				vec4 barycentric(b1, b2, b3, wn);
				uint32_t color;
				bool discard = shader.fragment(barycentric, color);

				if (!discard && wn < z_buffer[i + (j * frame.w)])
				{
					z_buffer[i + (j * frame.w)] = wn;
					frame.set_pixel(i, j, color);
				}
			}
		}
	}
}

void Renderer::draw_wireframe_triangle(std::vector<vec4> coords)
{
    draw_line(coords[0].x, coords[0].y, coords[1].x, coords[1].y);
    draw_line(coords[1].x, coords[1].y, coords[2].x, coords[2].y);
    draw_line(coords[2].x, coords[2].y, coords[0].x, coords[0].y);
}

void Renderer::draw_line(int x0, int y0, int x1, int y1)
{
    int x = x0;
    int y = y0;
    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);
    bool steep = false;

    if (dy > dx)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        std::swap(x, y);
        std::swap(dx, dy);
        steep = true;
    }

    int P = 2*dy - dx;

    bool decrement_x = (x0 > x1);
    bool decrement_y = (y0 > y1);

    while ((decrement_x) ? x >= x1 : x <= x1)
    {
        if (steep)
        {
            frame.set_pixel(y, x, 0xffff0000);
        } else
        {
            frame.set_pixel(x, y, 0xffff0000); }
        if (P < 0)
        {
            P = P + 2*dy;
        } else
        {
            P = P + 2*dy - 2*dx;
            if (decrement_y)
            {
                --y;
            } else
            {
                ++y;
            }
        }
        
        if (decrement_x)
        {
            --x;
        } else
        {
            ++x;
        }
    }
}

void Renderer::setup_zbuffer()
{
	z_buffer = new float[frame.w * frame.h];
	for (int i = 0; i < frame.w * frame.h; i++)
	{
		z_buffer[i] = std::numeric_limits<float>::max();
	}
}