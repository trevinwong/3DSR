#include "rasterizer.h"
#include <limits>
#include <cmath>
#include <utility>
#include "utils.h"
#include "vertex.h"

Rasterizer::Rasterizer(World& w, Frame& f) :
    world(w), frame(f)
{
}

void Rasterizer::render()
{
    frame.fill_frame_with_color(0xADD8E6);
    setup_zbuffer();
    setup_vertices();
}

// "Vertex shader" step.
// Uses the world to begin setting up the information it needs to render onto the frame, by processing coordinates
void Rasterizer::setup_vertices()
{
    mat4 view = lookAt(world.get_eye(), world.get_look_at_pt());

    // TODO: refactor these functions to be not hardcoded, pass in args
    mat4 perspective = get_perspective_matrix();
    mat4 viewport = get_viewport_matrix();

    for (auto&[mesh, model] : world.get_meshes_in_world())
    {
        mat4 mv = view * model;
        mat4 mv_t_inv = inverse(transpose(view * model));

        for (Polygon& polygon : mesh.polygons)
        {
            // Process vertex attributes.
            for (Vertex& vertex : polygon.vertices)
            {
                vec4 homogeneous_coords = vertex.local_coords.homogenize();
                vec4 world_coords = model * homogeneous_coords;
                vec4 clip_coords = perspective * view * world_coords;

                // We want to keep -z around. It'll let us do perspective interpolation.
                float w = clip_coords.w;
                vec4 screen_coords = clip_coords / clip_coords.w;
                vec4 viewport_coords = viewport * screen_coords;

                // We need to convert to int otherwise weird black gaps will appear between triangles.
                viewport_coords.x = (int) viewport_coords.x;
                viewport_coords.y = (int) viewport_coords.y;
                viewport_coords.w = w;
                
                // Calculate intensity for Gouraud shading.
                // Directional light, not point light.
                // TODO: should not be called get_light, but rather get_light_dir, if thats what we intend, since its already normalized
                
                // For point light.
                /* vec3 to_light = world.get_light() - world_coords; */

                float intensity = std::max(dot(vertex.normal, world.get_light()), 0.1f);

                // Attenuation can be added too.
                // This makes the model really dark though. Apparently quadratic attenuation is rarely used so the formula is:
                // 1.0 / (1 + attenuation coefficient*distance to light)
                // http://math.hws.edu/graphicsbook/c7/s2.html 7.27
                
                // TODO: Refactor this weird homogenizing thing
                vec4 perspective_correct_normal = vertex.normal.homogenize();
                // A vector is apparently supposed to have 0 in w.
                perspective_correct_normal.w = 0; 

                // When transforming vertices, we also need to transform normals, by the transpose inverse of the matrix we use.
                // We only use the view * model matrix to do this however.
                // https://graphics.stanford.edu/courses/cs248-05/persp/persp2.html
                
                // Save everything back to the vertex.
                vertex.world_coords = world_coords;
                vertex.clip_coords = clip_coords;
                vertex.viewport_coords = viewport_coords;
                vertex.intensity = intensity;
                vertex.perspective_correct_normal = mv_t_inv * perspective_correct_normal;
            }

            // Calculate the normal of the face.
            // TODO: This could probably be done earlier to save unnecessary computation of vertex attributes.
            vec3 face_normal = cross(polygon.vertices[1].world_coords - polygon.vertices[0].world_coords, polygon.vertices[2].world_coords - polygon.vertices[0].world_coords).normalize();

            vec3 viewing = polygon.vertices[0].world_coords - world.get_eye();

            float facing = dot(face_normal, viewing);
            // If facing is positive, that means the polygon is facing the same direction as the viewing vector, aka, AWAY, so don't draw it!
            if (facing < 0)
            {
                // TODO: Refactor this?
                // This is no longer setting up vertices. we're getting into the fragment shader now.
                draw_triangle(polygon.vertices[0], polygon.vertices[1], polygon.vertices[2], mesh.texture);
            } 
        }
    }

}

void Rasterizer::draw_triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2, Texture* texture) 
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
mat4 Rasterizer::lookAt(vec3 eye, vec3 target, vec3 up)
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
mat4 Rasterizer::get_perspective_matrix()
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

    // Construct the viewport matrix, which comprises of multiple steps.
    // Remember, our NDCS coords range from -1 to 1. We want them to range from 0 to 1.
    // So we do this: (coords + 1), so it ranges from 0 to 2, and then divide by 2, which gets us to our desired range.
    // Finally, we multiply by our width and height of our viewport!
    // It all looks like this, using x as an example: width * (x + 1)/2 -> width*x/2 + width/2, which is exactly what's happening here
mat4 Rasterizer::get_viewport_matrix()
{
    int d = 255;
    mat4 viewport   (   frame.w/2, 0, 0, (frame.w)/2,
                        0, frame.h/2, 0, (frame.h)/2,
                        0, 0, d/2, d/2,
                        0, 0, 0, 1
                    );
    return viewport;
}

void Rasterizer::setup_zbuffer()
{
    z_buffer = new float[frame.w * frame.h];
    for (int i = 0; i < frame.w * frame.h; i++)
    {
            z_buffer[i] = std::numeric_limits<float>::max();
    }
}

void Rasterizer::draw_line(int x0, int y0, int x1, int y1, Frame& frame)
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


void Rasterizer::draw_line(vec3& v0, vec3& v1, vec3& v2, Frame& frame)
{
    draw_line(v0.x, v0.y, v1.x, v1.y, frame);
    draw_line(v1.x, v1.y, v2.x, v2.y, frame);
    draw_line(v2.x, v2.y, v0.x, v0.y, frame);
}
