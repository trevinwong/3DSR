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
    
    // TODO: refactor these functions to be not hardcoded, pass in args
    mat4 view = lookAt(world.get_eye(), world.get_look_at_pt());
    mat4 perspective = get_perspective_matrix();
    mat4 viewport = get_viewport_matrix();

    for (auto&[mesh, model] : world.get_meshes_in_world())
    {
        mat4 model_view = view * inverse(model);
        mat4 mv_t_inv = inverse(transpose(view * model));

        for (Face& face : mesh.getFaces())
        {
            std::vector<vec4> face_ndc;
            std::vector<Vertex> vertices_to_rasterize;

            for (Vertex& vertex : face.vertices)
            {
                vec4 clip_coords = perspective * model_view * vertex.position;
                vec4 normalized_device_coords = clip_coords / clip_coords.w; 
                face_ndc.push_back(normalized_device_coords);

                vec4 viewport_coords = viewport * normalized_device_coords;
                viewport_coords.x = (int) viewport_coords.x; // Convert to int to avoid black gaps between triangles.
                viewport_coords.y = (int) viewport_coords.y; // Convert to int to avoid black gaps between triangles.
                viewport_coords.w = clip_coords.w; // Keep -z for perspective-correct linear interpolation.
                 
                Vertex to_rasterize;
                to_rasterize.position = viewport_coords;
                to_rasterize.normal = inverse(transpose(model_view)) * vertex.normal; // Foundations of 3D Computer Graphics, 3.6
                to_rasterize.uv = vertex.uv;

                // DEPRECATED: use to_rasterize
                vertex.viewport_coords = viewport_coords;
                vertex.perspective_correct_normal = to_rasterize.normal;
            }

            // Foundations of 3D Computer Graphics, 12.2
            float backface = ((face_ndc[2].x - face_ndc[1].x) * (face_ndc[0].y - face_ndc[1].y)) -
                             ((face_ndc[2].y - face_ndc[1].y) * (face_ndc[0].x - face_ndc[1].x));
            if (backface > 0) { draw_triangle(face.vertices[0], face.vertices[1], face.vertices[2], mesh.getTexture()); }
        }
    }
}

void Renderer::draw_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, const std::shared_ptr<Texture>& texture) 
{
    // Create a vec2 out of each vertice's viewport coordinates - which will make calculations a lot easier
    vec2 vp0(v0.viewport_coords);
    vec2 vp1(v1.viewport_coords);
    vec2 vp2(v2.viewport_coords); 

    vec2 edge0 = (vp1 - vp0);
    vec2 edge1 = (vp2 - vp1);
    vec2 edge2 = (vp0 - vp2);

    // Get the bounding box of these points.
    int min_x = min3(vp0.x, vp1.x, vp2.x);
    int max_x = max3(vp0.x, vp1.x, vp2.x);
    int min_y = min3(vp0.y, vp1.y, vp2.y);
    int max_y = max3(vp0.y, vp1.y, vp2.y);

    // Clip against the screen.
    min_x = std::max(min_x, 0);
    max_x = std::min(max_x, frame.w - 1);
    min_y = std::max(min_y, 0);
    max_y = std::min(max_y, frame.h - 1);

    // Iterate through all pixels inside the bounding box.
    for (int i = min_x; i <= max_x; i++)
    {
        for (int j = min_y; j <= max_y; j++)
        {
            vec2 to_point_from_p0 = vec2(i, j) - vec2(vp0);
            vec2 to_point_from_p1 = vec2(i, j) - vec2(vp1);
            vec2 to_point_from_p2 = vec2(i, j) - vec2(vp2);

            float v0v1p = cross(edge0, to_point_from_p0); 
            float v1v2p = cross(edge1, to_point_from_p1); 
            float v2v0p = cross(edge2, to_point_from_p2); 

            if (v0v1p >= 0 && v1v2p >= 0 && v2v0p >= 0)
            {
                float v0v1v2 = cross(vp1 - vp0, vp2 - vp0);

                // Discard degenerate triangles.
                if (v0v1v2 == 0) return;

                // Calculate barycentric coordinates. 
                // We can leave out the division by 2 since we're just getting the ratio between the sub-triangle and the triangle.
                float w0 = v1v2p / v0v1v2;
                float w1 = v2v0p / v0v1v2;
                float w2 = v0v1p / v0v1v2;

                // We can only linearly interpolate for inverted z using barycentric coordinates
                // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
                float inverted_z = 0.0f;
                inverted_z += (w0 * (1.0f/v0.viewport_coords.w));
                inverted_z += (w1 * (1.0f/v1.viewport_coords.w));
                inverted_z += (w2 * (1.0f/v2.viewport_coords.w));

                float z = (1.0f/inverted_z);

                vec2 uv;
                vec2 uv0 = v0.texture_coords;
                vec2 uv1 = v1.texture_coords;
                vec2 uv2 = v2.texture_coords;

                uv0 /= v0.viewport_coords.w;
                uv1 /= v1.viewport_coords.w;
                uv2 /= v2.viewport_coords.w;

                uv += (uv0 * w0);
                uv += (uv1 * w1);
                uv += (uv2 * w2);

                uv *= z;

                // Interpolate the intensity of the color.
                float intensity = 0.0f;

                float intensity0 = v0.intensity;
                float intensity1 = v1.intensity;
                float intensity2 = v2.intensity;

                intensity0 /= v0.viewport_coords.w;
                intensity1 /= v1.viewport_coords.w;
                intensity2 /= v2.viewport_coords.w;

                intensity += (intensity0 * w0);
                intensity += (intensity1 * w1);
                intensity += (intensity2 * w2);
                
                intensity *= z;

                // Interpolate normals.
                vec3 normal;

                vec3 normal0 = v0.normal;
                vec3 normal1 = v1.normal;
                vec3 normal2 = v2.normal;

                normal0 /= v0.viewport_coords.w;
                normal1 /= v1.viewport_coords.w;
                normal2 /= v2.viewport_coords.w;

                normal += (normal0 * w0);
                normal += (normal1 * w1);
                normal += (normal2 * w2);
                
                normal *= z;
                normal.normalize_inplace();

                // Interpolate vertex positions.
                // We're repeating the perspective division process and using it for interpolation for a given pixel.
                vec2 xy;

                vec2 clip_xy0 = v0.clip_coords;
                vec2 clip_xy1 = v1.clip_coords;
                vec2 clip_xy2 = v2.clip_coords;

                clip_xy0 /= v0.viewport_coords.w;
                clip_xy1 /= v1.viewport_coords.w;
                clip_xy2 /= v2.viewport_coords.w;

                xy += (clip_xy0 * w0);
                xy += (clip_xy1 * w1);
                xy += (clip_xy2 * w2);
                
                xy *= z;

                // If there is no texture, just use white as the default color.
                float r = 255.0f;
                float g = 255.0f;
                float b = 255.0f;

                if (texture != nullptr)
                {
                    // The u,v coordinates HAVE to be floored before indexing the texture color with them!
                    uint32_t u = (uint32_t) std::floor(uv.x * texture->width);
                    uint32_t v = (uint32_t) std::floor(uv.y * texture->width);
     
                    // Sample the color at uv.x and uv.y.
                    // Nice way of sampling borrowed from NotCamelCase/SoftLit.
                    // TODO: Abstract getting texture color (won't always be in RGB fashion)
                    int idx = ((v * texture->width) + u) * texture->channels;
                    r = (float) texture->data[idx++];
                    g = (float) texture->data[idx++];
                    b = (float) texture->data[idx++];

                    uint32_t texture_color = SDL_MapRGBA(frame.pixel_format, r, g, b, 255);
                    uint32_t texture_shaded_color = SDL_MapRGBA(frame.pixel_format, r * intensity, g * intensity, b * intensity, 255);
                }

                uint32_t gouraud_color = SDL_MapRGBA(frame.pixel_format, intensity * 255, intensity * 255, intensity * 255, 255);

                // Phong shading
                float kd = 0.5f;
                float ks = 0.6f;

                vec3 fragment = vec3(xy.x, xy.y, z);
                fragment.normalize_inplace();

                vec3 to_eye = world.get_eye() - fragment;
                to_eye.normalize_inplace();
                vec3 to_light = world.get_light() - fragment;
                to_light.normalize_inplace();
                // https://www.cs.utexas.edu/~bajaj/graphics2012/cs354/lectures/lect14.pdf
                // slide 7
                vec3 reflected = (normal - to_light)  * (dot(normal, to_light)) * 2;
                reflected.normalize_inplace();

                float ambient = 0.3f;
                float diffuse = std::max(dot(normal, world.get_light()), 0.0f);
                float specular = std::pow(std::max(dot(to_eye, reflected), 0.0f), 2.0f);
                specular = std::max(std::pow(dot(to_eye, reflected), 8.0f), 0.0f);

                float phong_term = ambient + (kd * diffuse) + (ks * specular);

                uint32_t phong_combined = SDL_MapRGBA(frame.pixel_format, r * phong_term, g * phong_term, b * phong_term, 255);
                uint32_t just_phong = SDL_MapRGBA(frame.pixel_format, std::min(phong_term * 255, 255.0f), std::min(phong_term * 255, 255.0f), std::min(phong_term * 255, 255.0f), 255);

                // If the z-value of the pixel we're on is greater than our current stored z-buffer's value...
                if (z < z_buffer[i + (j * frame.w)])
                {
                    // Replace it and color that pixel in.
                    z_buffer[i + (j * frame.w)] = z;
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

    vec3 forward = eye - target;
    forward.normalize_inplace();

    // Be very careful with the order of these cross products.
    vec3 left = cross(up, forward);
    left.normalize_inplace();

    // Re-calculate our up vector using our forward and left vector.
    up = cross(forward, left);

    mat4 mR         (   left.x, up.x, forward.x, 0,
                        left.y, up.y, forward.y, 0,
                        left.z, up.z, forward.z, 0,
                        0, 0, 0, 1
                    );

    // Inverse of mR is equal to the transpose since all basis vectors are orthonormal.
    // Why do we want to take the inverse? This is the matrix that gives us the rotation of the camera. We want to unrotate it.
    // Since we've calculated the left vector, unrotating it will give us a camera facing down the -Z axis!!
    mat4 mR_transpose = transpose(mR); 
    mat4 mView = mR_transpose * mT;

    return mView;
}

// The math is from: http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective
// NOTE: this assumes n and f are both positive!!!
// NOTE: this transforms the coordinates into NDCS coordinates, which FLIPS the direction of our z-axis, essentially
// Our camera typically looks down the negative z-axis but after transforming the coordinates it looks down the positive z-axis.
// This means your z-buffer implementation needs to change: closer coordinates should be SMALLER, not LARGER
// Construct the perspective matrix.
mat4 Renderer::get_perspective_matrix()
{
    float t = 1.0f;
    float b = -t;
    float r = 1.0f;
    float l = -r;
    float n = 1.8f;
    float f = 10.0f;

    mat4 perspective(   2*n/(r-l), 0, (r+l)/(r-l), 0,
                        0, 2*n/(t-b), (t+b)/(t-b), 0,
                        0, 0, -(f + n)/(f - n), -2*(f * n)/(f - n),
                        0, 0, -1, 0
                    );
    return perspective;
}

// Transforms the canonical cube (which ranges from [-1,-1,-1] to [1,1,1]) to range from [0,0,0] to [W,H,1].
mat4 Renderer::get_viewport_matrix()
{
    mat4 viewport   (   
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


void Renderer::draw_line(vec3& v0, vec3& v1, vec3& v2, Frame& frame)
{
    draw_line(v0.x, v0.y, v1.x, v1.y, frame);
    draw_line(v1.x, v1.y, v2.x, v2.y, frame);
    draw_line(v2.x, v2.y, v0.x, v0.y, frame);
}
