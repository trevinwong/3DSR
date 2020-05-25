#include "renderer.h"
#include <limits>
#include <cmath>
#include <utility>
#include "utils.h"
#include "vertex.h"

// TODO: Rename to renderer.
// Separate vertex pre-processing/vertex shader from rasterization/fragment shading.

Renderer::Renderer(World& w, Frame& f) :
    world(w), frame(f)
{
}

void Renderer::render()
{
    frame.fill_frame_with_color(0xADD8E6);
    setup_zbuffer();
    
    mat4 view = lookAt(world.get_eye(), world.get_look_at_pt());
    mat4 perspective = get_perspective_matrix();
    mat4 viewport = get_viewport_matrix();

    for (auto&[mesh, model] : world.get_meshes_in_world())
    {
        mat4 model_view = view * model;

        for (Face& face : mesh.getFaces())
        {
            std::vector<Vertex> vrt_to_rasterize;

            for (Vertex& vertex : face.vertices)
            {
                vec4 world_coords = vertex.position;
                vec4 clip_coords = perspective * model_view * vertex.position;
                vec4 normalized_device_coords = clip_coords / clip_coords.w; 

                vec4 viewport_coords = viewport * normalized_device_coords;
                viewport_coords.x = (int) viewport_coords.x; // Convert to int to avoid black gaps between triangles.
                viewport_coords.y = (int) viewport_coords.y; // Convert to int to avoid black gaps between triangles.
                viewport_coords.w = clip_coords.w; // Keep wn (aka -z) for perspective-correct linear interpolation.
                 
                float intensity = std::max(dot(vertex.normal, world.get_light()), 0.1f);
                Vertex to_rasterize;
                to_rasterize.position = viewport_coords;
                to_rasterize.normal = inverse(transpose(model_view)) * vertex.normal; // Foundations of 3D Computer Graphics, 3.6
                to_rasterize.uv = vertex.uv;
                to_rasterize.world_coords = world_coords;
                to_rasterize.intensity = intensity;

                vrt_to_rasterize.push_back(to_rasterize);
            }

            // Foundations of 3D Computer Graphics, 12.2
            // Calculate the direction of the normal of the screen-space triangle, which is either towards -wn or +wn
            // If +wn: vertices are CCW, aka facing the front-side
            // If -wn: vertices are CW, aka facing the back-side
            float backface = ((vrt_to_rasterize[2].position.x - vrt_to_rasterize[1].position.x) * (vrt_to_rasterize[0].position.y - vrt_to_rasterize[1].position.y)) -
                             ((vrt_to_rasterize[2].position.y - vrt_to_rasterize[1].position.y) * (vrt_to_rasterize[0].position.x - vrt_to_rasterize[1].position.x));
            if (backface > 0) { draw_triangle(vrt_to_rasterize[0], vrt_to_rasterize[1], vrt_to_rasterize[2], mesh.getTexture()); }
        }
    }
}

void Renderer::draw_triangle(const Vertex& v1, const Vertex& v2, const Vertex& v3, const std::shared_ptr<Texture>& texture) 
{
    vec2 v1_2d(v1.position);
    vec2 v2_2d(v2.position);
    vec2 v3_2d(v3.position); 

    vec2 edge0 = (v2_2d - v1_2d);
    vec2 edge1 = (v3_2d - v2_2d);
    vec2 edge2 = (v1_2d - v3_2d);

    int min_x = std::max(min3(v1_2d.x, v2_2d.x, v3_2d.x), 0);
    int max_x = std::min(max3(v1_2d.x, v2_2d.x, v3_2d.x), frame.w - 1);
    int min_y = std::max(min3(v1_2d.y, v2_2d.y, v3_2d.y), 0);
    int max_y = std::min(max3(v1_2d.y, v2_2d.y, v3_2d.y), frame.h - 1);

    for (int i = min_x; i <= max_x; i++)
    {
        for (int j = min_y; j <= max_y; j++)
        {
            vec2 point(i,j);
            vec2 to_point_from_p1 = point - vec2(v1_2d);
            vec2 to_point_from_p2 = point - vec2(v2_2d);
            vec2 to_point_from_p3 = point - vec2(v3_2d);

            float v0v1p = cross(edge0, to_point_from_p1); 
            float v1v2p = cross(edge1, to_point_from_p2); 
            float v2v0p = cross(edge2, to_point_from_p3); 

            if (v0v1p >= 0 && v1v2p >= 0 && v2v0p >= 0)
            {
                float v0v1v2 = cross(v2_2d - v1_2d, v3_2d - v1_2d);
                if (v0v1v2 == 0) return; // Discard degenerate triangles, whose area is 0.

                // Barycentric coordinates, division by 2 is left out b/c only the ratio is needed
                float b1 = v1v2p / v0v1v2;
                float b2 = v2v0p / v0v1v2;
                float b3 = v0v1p / v0v1v2;

                // Rational linear interpolation.
                // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
                // Foundations of 3D Computer Graphics, Ch 13
                float wn_reciprocal = (b1 * (1.0f/v1.position.w)) + (b2 * (1.0f/v2.position.w)) + (b3 * (1.0f/v3.position.w));
                float wn = (1.0f/wn_reciprocal);

                // TODO: abstract shaders and varying variables
                vec2 uv(((v1.uv/v1.position.w) * wn * b1) + ((v2.uv/v2.position.w) * wn * b2) + ((v3.uv/v3.position.w) * wn * b3));
                vec3 normal(((v1.normal/v1.position.w) * wn * b1) + ((v2.normal/v2.position.w) * wn * b2) + ((v3.normal/v3.position.w) * wn * b3));
                vec3 world_pos(((v1.world_coords/v1.position.w) * wn * b1) + ((v2.world_coords/v2.position.w) * wn * b2) + ((v3.world_coords/v3.position.w) * wn * b3));
                float intensity(((v1.intensity/v1.position.w) * wn * b1) + ((v2.intensity/v2.position.w) * wn * b2) + ((v3.intensity/v3.position.w) * wn * b3));

                float r = 255.0f, g = 255.0f, b = 255.0f;
                if (texture != nullptr)
                {
                    uint32_t u = (uint32_t) std::floor(uv.x * texture->width);
                    uint32_t v = (uint32_t) std::floor(uv.y * texture->height);
     
                    // NotCamelCase/SoftLit/Texture.cpp
                    int idx = ((v * texture->width) + u) * texture->channels;
                    r = (float) texture->data[idx++];
                    g = (float) texture->data[idx++];
                    b = (float) texture->data[idx++];
                }

                float ka = 0.1f;
                float kd = 0.3f;
                float ks = 0.6f;

                vec3 fragment = world_pos;
                vec3 to_eye = (world.get_eye() - fragment).normalize();
                vec3 to_light = (world.get_light() - fragment).normalize();
                // https://www.cs.utexas.edu/~bajaj/graphics2012/cs354/lectures/lect14.pdf
                // slide 7
                normal.normalize_inplace();
                vec3 proj_of_to_light_on_normal = (normal * dot(to_light, normal));
                vec3 to_reflection_pos = (proj_of_to_light_on_normal - to_light) * 2;
                vec3 reflected = to_light + to_reflection_pos;

                float diffuse = std::max(dot(normal, to_light), 0.0f);
                float specular = std::pow(std::max(dot(to_eye, reflected), 0.0f), 2.0f);

                float phong_term = ka + (kd * diffuse) + (ks * specular);

                uint32_t phong_combined = SDL_MapRGBA(frame.pixel_format, r * phong_term, g * phong_term, b * phong_term, 255);
                uint32_t just_phong = SDL_MapRGBA(frame.pixel_format, std::min(phong_term * 255, 255.0f), std::min(phong_term * 255, 255.0f), std::min(phong_term * 255, 255.0f), 255);
                uint32_t gouraud_color = SDL_MapRGBA(frame.pixel_format, intensity * 255, intensity * 255, intensity * 255, 255);

                if (wn < z_buffer[i + (j * frame.w)])
                {
                    z_buffer[i + (j * frame.w)] = wn;
                    frame.set_pixel(i, j, phong_combined);
                }
            }
        }
    }
}

// Constructs a view matrix given the position of the eye and the target.
// Use the unit vector pointing up as our temporary up vector, if none is specified.
mat4 Renderer::lookAt(vec3 eye, vec3 target, vec3 up)
{
    // Translate eye position back to origin.
    // Our calculations assume that our eye is positioned at the origin and looks down the -Z axis.
    mat4 mT         (   1, 0, 0, -eye.x,
                        0, 1, 0, -eye.y,
                        0, 0, 1, -eye.z,
                        0, 0, 0, 1
                    );

    vec3 forward = (eye - target).normalize();

    // Be very careful with the order of these cross products.
    vec3 left = cross(up.normalize(), forward).normalize();

    // Re-calculate our up vector using our forward and left vector.
    up = cross(forward, left);

    mat4 mR         (   left.x, up.x, forward.x, 0,
                        left.y, up.y, forward.y, 0,
                        left.z, up.z, forward.z, 0,
                        0, 0, 0, 1
                    );

    return inverse(mR) * mT;
}

// The math is from: http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective
// NOTE: this assumes n and f are both positive!!!
// NOTE: this transforms the coordinates into NDCS coordinates, which FLIPS the direction of our wn-axis, essentially
// Our camera typically looks down the negative wn-axis but after transforming the coordinates it looks down the positive wn-axis.
// This means your wn-buffer implementation needs to change: closer coordinates should be SMALLER, not LARGER
// Construct the perspective matrix.
mat4 Renderer::get_perspective_matrix()
{
    float t = 1.0f;
    float b = -t;
    float r = 1.0f;
    float l = -r;
    float n = 1.8f;
    float f = 10.0f;

    mat4 perspective(   
                        2*n/(r-l), 0,         (r+l)/(r-l),      0,
                        0,         2*n/(t-b), (t+b)/(t-b),      0,
                        0,         0,         -(f + n)/(f - n), -2*(f * n)/(f - n),
                        0,         0,         -1,               0
                    );
    return perspective;
}

// Transforms the canonical cube (which ranges from [-1,-1,-1] to [1,1,1]) to range from [0,0,0] to [W,H,1].
mat4 Renderer::get_viewport_matrix()
{
    mat4 viewport(   
                    frame.w/2, 0,         0,   (frame.w)/2,
                    0,         frame.h/2, 0,   (frame.h)/2,
                    0,         0,         1/2, 1/2,
                    0,         0,         0,   1
                );
    return viewport;
}

void Renderer::setup_zbuffer()
{
    z_buffer = new float[frame.w * frame.h];
    for (int i = 0; i < frame.w * frame.h; i++)
    {
            z_buffer[i] = std::numeric_limits<float>::max();
    }
}

void Renderer::draw_line(int x0, int y0, int x1, int y1, Frame& frame)
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


void Renderer::draw_line(vec3& v1, vec3& v2, vec3& v3, Frame& frame)
{
    draw_line(v1.x, v1.y, v2.x, v2.y, frame);
    draw_line(v2.x, v2.y, v3.x, v3.y, frame);
    draw_line(v3.x, v3.y, v1.x, v1.y, frame);
}
