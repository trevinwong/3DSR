#include <iostream>
#include <limits>
#include <utility>
#include <cmath>

#define PERSPECTIVE
#define MODEL
// #define ZBUFFER

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "frame.h"
#include "texture.h"
#include "utils.h"
#include "mesh.h"

#define trace(var)  { std::cout << "Line " << __LINE__ << ": " << #var << "=" << var << "\n";}

#include "SDL.h"

const inline int WINDOW_WIDTH = 800;
const inline int WINDOW_HEIGHT = 800;
const inline SDL_PixelFormat* pixel_format;

bool quit = false;

void draw_line(int x0, int y0, int x1, int y1, Frame& frame)
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

void draw_line(vec3& v0, vec3& v1, vec3& v2, Frame& frame)
{
    draw_line(v0.x, v0.y, v1.x, v1.y, frame);
    draw_line(v1.x, v1.y, v2.x, v2.y, frame);
    draw_line(v2.x, v2.y, v0.x, v0.y, frame);
}

// We are given two vectors with z = 0, and want to calculate their cross product.
// This is equivalent to the 2x2 determinant formed by the vectors created by their x and y components.
// This is also equivalent to the area of the parallelogram formed by the two vectors.
// This is also equivalent to comparing b to the edge function of a.
float cross_2d(const vec2& a, const vec2& b)
{
    return (a.x * b.y) - (a.y * b.x);
}


// Assume p0, p1 and p2 are listed in CCW order, and we're using a right-hand coordinate system.
// TODO: Clean up parameters.
void draw_triangle(vec4 p0, vec4 p1, vec4 p2, Frame& frame, float* z_buffer, std::vector<vec2> uvs, Texture texture, std::vector<float> intensities, uint32_t color, std::vector<vec3> vertex_colors, std::vector<vec3> normals, vec3 eye, vec3 light, std::vector<vec2> orig_xy)
{
    /* p0.print(); */
    /* p1.print(); */
    /* p2.print(); */

    vec2 edge0 = (p1 - p0);
    vec2 edge1 = (p2 - p1);
    vec2 edge2 = (p0 - p2);

    // Get the bounding box of these points.
    int min_x = min3(p0.x, p1.x, p2.x);
    int max_x = max3(p0.x, p1.x, p2.x);
    int min_y = min3(p0.y, p1.y, p2.y);
    int max_y = max3(p0.y, p1.y, p2.y);

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
            vec2 to_point_from_p0 = vec2(i, j) - vec2(p0);
            vec2 to_point_from_p1 = vec2(i, j) - vec2(p1);
            vec2 to_point_from_p2 = vec2(i, j) - vec2(p2);

            /*
            edge0.print();
            to_point_from_p0.print();

            edge1.print();
            to_point_from_p1.print();

            edge2.print();
            to_point_from_p2.print();
            */
            float v0v1p = cross_2d(edge0, to_point_from_p0); 
            float v1v2p = cross_2d(edge1, to_point_from_p1); 
            float v2v0p = cross_2d(edge2, to_point_from_p2); 
            // std::cout << " v0v1p: " << v0v1p << " v1v2p: " << v1v2p << " v2v0p: " <<  v2v0p << std::endl;

            if (v0v1p >= 0 && v1v2p >= 0 && v2v0p >= 0)
            {
                float v0v1v2 = cross_2d(p1 - p0, p2 - p0);

                if (v0v1v2 == 0) return;

                // Calculate barycentric coordinates. We can leave out the division by 2 since we're just getting the ratio between the sub-triangle and the triangle.
                // TODO: Be careful about dividing by 0.
                // aka check for degenerate triangles first?
                float w0 = v1v2p / v0v1v2; // corresponds to the weight of p0
                float w1 = v2v0p / v0v1v2; // corresponds to the weight of p1
                float w2 = v0v1p / v0v1v2; // corresponds to the weight of p2

                // We can only linearly interpolate for inverted z using barycentric coordinates
                // https://www.scratchapixel.com/lessons/3d-basic-rendering/rasterization-practical-implementation/visibility-problem-depth-buffer-depth-interpolation
                float inverted_z = 0.0f;
                inverted_z += (w0 * (1.0f/p0.w));
                inverted_z += (w1 * (1.0f/p1.w));
                inverted_z += (w2 * (1.0f/p2.w));

                float z = (1.0f/inverted_z);

                vec2 uv;
                vec2 uv0 = uvs[0];
                vec2 uv1 = uvs[1];
                vec2 uv2 = uvs[2];

                uv0 /= p0.w;
                uv1 /= p1.w;
                uv2 /= p2.w;

                uv += (uv0 * w0);
                uv += (uv1 * w1);
                uv += (uv2 * w2);

                uv *= z;

                // Interpolate the intensity of the color.
                float intensity = 0;

                float intensity0 = intensities[0];
                float intensity1 = intensities[1];
                float intensity2 = intensities[2];

                intensity0 /= p0.w;
                intensity1 /= p1.w;
                intensity2 /= p2.w;

                intensity += (intensity0 * w0);
                intensity += (intensity1 * w1);
                intensity += (intensity2 * w2);
                
                intensity *= z;

                // Interpolate normals.
                vec3 normal;

                vec3 normal0 = normals[0];
                vec3 normal1 = normals[1];
                vec3 normal2 = normals[2];

                normal0 /= p0.w;
                normal1 /= p1.w;
                normal2 /= p2.w;

                normal += (normal0 * w0);
                normal += (normal1 * w1);
                normal += (normal2 * w2);
                
                normal *= z;
                normal.normalize_inplace();

                // Interpolate vertex positions
                // We need to repeat the perspective division process.
                vec2 xy;

                vec2 orig_xy0 = orig_xy[0];
                vec2 orig_xy1 = orig_xy[1];
                vec2 orig_xy2 = orig_xy[2];

                orig_xy0 /= p0.w;
                orig_xy1 /= p1.w;
                orig_xy2 /= p2.w;

                xy += (orig_xy0 * w0);
                xy += (orig_xy1 * w1);
                xy += (orig_xy2 * w2);
                
                xy *= z;

                // Interpolate vertex colors.
                vec3 color;

                vertex_colors[0] /= p0.z;
                vertex_colors[1] /= p1.z;
                vertex_colors[2] /= p2.z;

                color += (vertex_colors[0] * w0);
                color += (vertex_colors[1] * w1);
                color += (vertex_colors[2] * w2);

                color *= z;

                // The u,v coordinates HAVE to be floored before indexing the texture color with them!
                uint32_t u = (uint32_t) std::floor(uv.x * texture.width);
                uint32_t v = (uint32_t) std::floor(uv.y * texture.width);
 
                // Sample the color at uv.x and uv.y.
                // Nice way of sampling borrowed from NotCamelCase/SoftLit.
                // TODO: Abstract getting texture color (won't always be in RGB fashion)
                int idx = ((v * texture.width) + u) * texture.channels;
                float r = (float) texture.data[idx++];
                float g = (float) texture.data[idx++];
                float b = (float) texture.data[idx++];
                uint32_t texture_color = SDL_MapRGBA(pixel_format, r, g, b, 255);
                uint32_t texture_shaded_color = SDL_MapRGBA(pixel_format, r * intensity, g * intensity, b * intensity, 255);
                uint32_t gouraud_color = SDL_MapRGBA(pixel_format, intensity * 255, intensity * 255, intensity * 255, 255);
                uint32_t interpolated_colors = SDL_MapRGBA(pixel_format, color.x, color.y, color.z, 255);

                // Phong shading
                float kd = 0.5f;
                float ks = 0.6f;

                vec3 fragment = vec3(xy.x, xy.y, z);
                fragment.normalize_inplace();

                vec3 to_eye = eye - fragment;
                to_eye.normalize_inplace();
                vec3 to_light = light - fragment;
                to_light.normalize_inplace();
                // https://www.cs.utexas.edu/~bajaj/graphics2012/cs354/lectures/lect14.pdf
                // slide 7
                vec3 reflected = (normal - to_light)  * (dot(normal, to_light)) * 2;
                reflected.normalize_inplace();

                float ambient = 0.1f;
                float diffuse = std::max(dot(normal, light), 0.0f);
                float specular = std::pow(std::max(dot(to_eye, reflected), 0.0f), 2.0f);
                specular = std::max(std::pow(dot(to_eye, reflected), 8.0f), 0.0f);

                float phong_term = ambient + (kd * diffuse) + (ks * specular);

                uint32_t phong_combined = SDL_MapRGBA(pixel_format, r * phong_term, g * phong_term, b * phong_term, 255);
                uint32_t just_phong = SDL_MapRGBA(pixel_format, std::min(phong_term * 255, 255.0f), std::min(phong_term * 255, 255.0f), std::min(phong_term * 255, 255.0f), 255);

                // If the z-value of the pixel we're on is greater than our current stored z-buffer's value...
                #ifdef PERSPECTIVE
                if (z < z_buffer[i + (j * frame.w)])
                #else
                if (z > z_buffer[i + (j * frame.w)])
                #endif
                {
                    // Replace it and color that pixel in.
                    z_buffer[i + (j * frame.w)] = z;
                    frame.set_pixel(i, j, phong_combined);
                }
            }
        }
    }
}

void draw_triangle_simple(const std::vector<vec3>& vertices, const std::vector<vec3>& colors, Frame& frame, float* z_buffer, std::vector<vec2> uvs, Texture& texture)
{
    vec3 p0 = vertices[0];
    vec3 p1 = vertices[1];
    vec3 p2 = vertices[2];

    vec2 edge0 = (p1 - p0);
    vec2 edge1 = (p2 - p1);
    vec2 edge2 = (p0 - p2);

    // Get the bounding box of these points.
    int min_x = min3(p0.x, p1.x, p2.x);
    int max_x = max3(p0.x, p1.x, p2.x);
    int min_y = min3(p0.y, p1.y, p2.y);
    int max_y = max3(p0.y, p1.y, p2.y);

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
            vec2 to_point_from_p0 = vec2(i, j) - vec2(p0);
            vec2 to_point_from_p1 = vec2(i, j) - vec2(p1);
            vec2 to_point_from_p2 = vec2(i, j) - vec2(p2);

            float v0v1p = cross_2d(edge0, to_point_from_p0); 
            float v1v2p = cross_2d(edge1, to_point_from_p1); 
            float v2v0p = cross_2d(edge2, to_point_from_p2); 

            if (v0v1p >= 0 && v1v2p >= 0 && v2v0p >= 0)
            {
                float v0v1v2 = cross_2d(p1 - p0, p2 - p0);

                if (v0v1v2 == 0) return;

                float w0 = v1v2p / v0v1v2; // corresponds to the weight of p0
                float w1 = v2v0p / v0v1v2; // corresponds to the weight of p1
                float w2 = v0v1p / v0v1v2; // corresponds to the weight of p2

                // Get z value by interpolating from each vertice using the barycentric coordinates.
                // Should no longer work with perspective.
                float z = 0;
                z += (w0 * p0.z);
                z += (w1 * p1.z);
                z += (w2 * p2.z);

#ifdef PERSPECTIVE
                float inverted_z = 0;
                inverted_z += (w0 * 1/p0.z);
                inverted_z += (w1 * 1/p1.z);
                inverted_z += (w2 * 1/p2.z);

                z = 1/inverted_z;
#endif
                vec3 color;
                vec2 uv;

                vec3 color0 = colors[0];
                vec3 color1 = colors[1];
                vec3 color2 = colors[2];
                
                vec2 uv0 = uvs[0];
                vec2 uv1 = uvs[1];
                vec2 uv2 = uvs[2];

#ifdef PERSPECTIVE
                color0 /= p0.z;
                color1 /= p1.z;
                color2 /= p2.z;

                uv0 /= p0.z;
                uv1 /= p1.z;
                uv2 /= p2.z;
#endif
                // Interpolate vertex colors.
                color += (color0 * w0);
                color += (color1 * w1);
                color += (color2 * w2);

                uv += (uv0 * w0);
                uv += (uv1 * w1);
                uv += (uv2 * w2);
#ifdef PERSPECTIVE
                // Multiply the result by the z of the pixel
                color *= z;
                uv *= z;
#endif

                // The u,v coordinates HAVE to be floored before indexing the texture color with them!
                uint32_t u = (uint32_t) std::floor(uv.x * texture.width);
                uint32_t v = (uint32_t) std::floor(uv.y * texture.width);
 
                // Sample the color at uv.x and uv.y.
                // Nice way of sampling borrowed from NotCamelCase/SoftLit.
                // TODO: Abstract getting texture color (won't always be in RGB fashion)
                int idx = ((v * texture.width) + u) * texture.channels;
                float r = (float) texture.data[idx++];
                float g = (float) texture.data[idx++];
                float b = (float) texture.data[idx++];

                uint32_t interpolated_colors = SDL_MapRGBA(pixel_format, color.x, color.y, color.z, 255);
                uint32_t texture_color = SDL_MapRGBA(pixel_format, r, g, b, 255);

                // If the z-value of the pixel we're on is greater than our current stored z-buffer's value...
                #ifdef PERSPECTIVE
                if (z < z_buffer[i + (j * frame.w)])
                #else
                if (z > z_buffer[i + (j * frame.w)])
                #endif
                {
                    // Replace it and color that pixel in.
                    z_buffer[i + (j * frame.w)] = z;
                    frame.set_pixel(i, j, texture_color);
                }
            }
        }
    }
}

// Constructs a view matrix given the position of the eye and the target.
// Use the unit vector pointing up as our temporary up vector, if none is specified.
mat4 lookAt(vec3 eye, vec3 target, vec3 up = vec3(0, 1, 0))
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

int main() {
    // Initialize event var. to store events when unqueueing them.
    SDL_Event event;

    // Initialize SDL (we only care about the video subsystem)
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return 1;

    SDL_Window* window = SDL_CreateWindow("3DSR",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    WINDOW_WIDTH, WINDOW_HEIGHT,
                    SDL_WINDOW_SHOWN);

    // Attach an SDL_Renderer to the window.
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    // Creates the "screen" as a texture which we will update with the frame buffer we will draw to using the CPU.
    // Each pixel will be a uint32. 
    // The most significant 8 bytes will be the alpha, then red, then green, then blue, as specified by the pixel format below.
    SDL_Texture* screen = SDL_CreateTexture(renderer,
                        SDL_PIXELFORMAT_ARGB8888,
                        SDL_TEXTUREACCESS_STREAMING,
                        WINDOW_WIDTH, WINDOW_HEIGHT);
    pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    
    // Create a Frame object, which allocates a buffer to draw to.
    Frame frame(WINDOW_WIDTH, WINDOW_HEIGHT);
    frame.fill_frame_with_color(0xADD8E6);
    
    // Load our object.
    // DEPRECATED
    std::string inputfile = "obj/african_head.obj";
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;  

    // LoadObj now only passes in 1 std::string for error handling for release v1.0.6.
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, inputfile.c_str());

    if (!err.empty()) {
      std::cerr << "Error: " << err << std::endl;
    }

    // Load our object.
    Mesh mesh("obj/african_head.obj");


    if (!ret) {
      exit(1);
    }

    // Create a light.
    // TODO: The light distance should affect the intensity of the shading...
    vec3 light_dir(0.5, -1, 1);
    light_dir.normalize_inplace();

    // Create an eye.
    vec3 eye(1, 1, 3);
    
    float t = 1.0f;
    float b = -t;
    float r = 1.0f;
    float l = -r;
    float n = 1.8f;
    float f = 10.0f;

    // Here's how the pipeline goes:
    // Local -> world -> eye -> clip -> screen -> viewport
    // We don't need to transform our local coordinates into our world coordinates since we have not transformed them in any way.                 
   
    // Construct our model matrix.
    // (i.e should contain any of the transformations we make to the model in the world, which right now is none)
    mat4 model      (   1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1
                    );
    
    // Point our eye towards the origin and construct a view matrix from that to convert to eye coords from world coords.
    mat4 view = lookAt(eye, vec3(0, 0, 0)); 

    // The math is from: http://www.songho.ca/opengl/gl_projectionmatrix.html#perspective
    // NOTE: this assumes n and f are both positive!!!
    // NOTE: this transforms the coordinates into NDCS coordinates, which FLIPS the direction of our z-axis, essentially
    // Our camera typically looks down the negative z-axis but after transforming the coordinates it looks down the positive z-axis.
    // This means your z-buffer implementation needs to change: closer coordinates should be SMALLER, not LARGER
    // Construct the perspective matrix.
    mat4 perspective(   2*n/(r-l), 0, (r+l)/(r-l), 0,
                        0, 2*n/(t-b), (t+b)/(t-b), 0,
                        0, 0, -(f + n)/(f - n), -2*(f * n)/(f - n),
                        0, 0, -1, 0
                    );

    // Construct the viewport matrix, which comprises of multiple steps.
    // Remember, our NDCS coords range from -1 to 1. We want them to range from 0 to 1.
    // So we do this: (coords + 1), so it ranges from 0 to 2, and then divide by 2, which gets us to our desired range.
    // Finally, we multiply by our width and height of our viewport!
    // It all looks like this, using x as an example: width * (x + 1)/2 -> width*x/2 + width/2, which is exactly what's happening here
    int d = 255;
    mat4 viewport   (   frame.w/2, 0, 0, (frame.w)/2,
                        0, frame.h/2, 0, (frame.h)/2,
                        0, 0, d/2, d/2,
                        0, 0, 0, 1
                    );

    // Model-view projection matrix. Note that we don't transform the object from local coords to world coords, so there is no model matrix needed!
    mat4 mvp = perspective * view * model;
    mat4 mv = view * model;
    mat4 mv_transpose_inv = inverse(transpose(mv));

    // Load our texture.
    Texture texture("img/african_head_diffuse.tga");

    // Time to draw the mesh. 
    // Let's allocate our z-buffer.
    float* z_buffer = new float[frame.w * frame.h];
    for (int i = 0; i < frame.w * frame.h; i++)
    {
        #ifdef PERSPECTIVE
            z_buffer[i] = std::numeric_limits<float>::max();
        #else 
            z_buffer[i] = std::numeric_limits<float>::lowest();
        #endif
    }

    std::vector<vec3> colors;
    colors.push_back(vec3(255, 0, 0));
    colors.push_back(vec3(0, 255, 0));
    colors.push_back(vec3(0, 0, 255));

    std::vector<vec2> uvs;
    uvs.push_back(vec2(0, 0));
    uvs.push_back(vec2(1, 0));
    uvs.push_back(vec2(0, 1));

#ifndef MODEL
#ifdef PERSPECTIVE
    std::vector<vec3> vertices;
    vertices.push_back((vec3(0.1*2, 0.2*2, 0.8)));
    vertices.push_back((vec3(0.5*2, 0.2*2, 1)));
    vertices.push_back((vec3(0.3*2, 0.4*2, 0.5)));


    std::vector<vec3> transformed;

    for (const vec3& vertex : vertices)
    {
        float vx = vertex.x;
        float vy = vertex.y;
        float vz = vertex.z;
        vec4 clip = mvp * vec4(vx, vy, vz, 1);
        vec4 screen = clip / clip.w; // Perspective divide.
        vec3 viewport_coord = (viewport * screen);
        viewport_coord.x = (int) viewport_coord.x;
        viewport_coord.y = (int) viewport_coord.y;
        transformed.push_back(viewport_coord);
    } 

    draw_triangle_simple(transformed, colors, frame, z_buffer, uvs, texture);

#ifdef ZBUFFER
    float old_low = std::numeric_limits<float>::max();
    float old_high = std::numeric_limits<float>::lowest();

    for (int i = 0; i < frame.w; i++)
    {
        for (int j = 0; j < frame.h; j++)
        {
            old_low = std::min(old_low, z_buffer[i+(j*frame.w)]);
            if (z_buffer[i+(j*frame.w)] != std::numeric_limits<float>::max())
            {
                old_high = std::max(old_high, z_buffer[i+(j*frame.w)]);
            }
        }
    }

    std::cout << old_low << std::endl;
    std::cout << old_high << std::endl;

    // Draw zbuffer
    for (int i = 0; i < frame.w; i++)
    {
        for (int j = 0; j < frame.h; j++)
        {
            float old_z = z_buffer[i+(j*frame.w)];
            float z = 0.0f;
            if (old_z != std::numeric_limits<float>::max())
            {
                z = convert_num_to_new_range(old_low, old_high, 0.0f, 255.0f, old_z);
            }
            if (z != 0.0f)
            {
                std::cout << "z: " << z << std::endl;
            }
            frame.set_pixel(i, j, SDL_MapRGBA(pixel_format, z, 0, 0, 255));
        }
    }
#endif
#else
    std::vector<vec3> vertices;
    vertices.push_back(vec3(381, 439, 78.6035));
    vertices.push_back(vec3(628, 400, 29.5712));
    vertices.push_back(vec3(521, 565, 81.4573));
    draw_triangle_simple(vertices, colors, frame, z_buffer, uvs, texture);
#endif 
#endif

    #ifdef MODEL

    // For each shape...
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // For each face...
        size_t f_index_begin = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // Find the number of vertices that describes it. 
            int num_vertices = shapes[s].mesh.num_face_vertices[f];

            std::vector<vec3> world_coords;
            std::vector<vec4> viewport_coords;
            std::vector<vec2> texture_coords;
            std::vector<vec3> normals;

            std::vector<vec2> world_xy;
            std::vector<vec2> modelview_xy;
            std::vector<vec2> clip_xy;
            
            // All vertices are stored in counter-clockwise order by default.
            for (size_t v = 0; v < num_vertices; v++)
            {
                // Fetch the index of the 1st/2nd/3rd... etc. vertex
                // This index is NOT the vertex index, but rather the index in mesh.indices.
                tinyobj::index_t idx = shapes[s].mesh.indices[f_index_begin + v]; 
                // We want to loop back to the first vertex if we are at the last vertex.
                tinyobj::index_t next_idx = shapes[s].mesh.indices[f_index_begin + (v+1)%num_vertices];

                // We can obtain the vertex index by calling idx.vertex_index.
                // All vertices are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index + 2];

                world_coords.push_back(vec3(vx, vy, vz));

                #ifdef PERSPECTIVE
                    vec4 clip = mvp * vec4(vx, vy, vz, 1);
                    // TODO: Do some clipping here. Not sure how to reconstruct clipped triangles just yet.
                    // CHECK IF -w < x < w and -w < y < w because afer dividing by w, -1 < x < 1, same with y
                    // If we throw away the vertex what do we do? where do we reconstruct the vertex?
                    if (clip.x < -clip.w) clip.x = -clip.w;
                    if (clip.x > clip.w) clip.x = clip.w;
                    if (clip.y < -clip.w) clip.y = -clip.w;
                    if (clip.y > clip.w) clip.y = clip.w;
                    if (clip.z < -clip.w) clip.z = -clip.w;
                    if (clip.z > clip.w) clip.z = clip.w;
                    float w = clip.w; // we want to keep -z cause it'll let us do perspective interpolation.
                    vec4 screen = clip / clip.w; // Perspective divide.

                    world_xy.push_back(vec2(vx, vy));
                    modelview_xy.push_back(vec2(mv * vec4(vx, vy, vz, 1)));
                    clip_xy.push_back(vec2(clip.x, clip.y));
                #else
                    vec4 screen = vec4(vx, vy, vz, 1); 
                #endif

                // TODO: Why is z not between -1 and 1??
                // Big think: maybe the z thats not between -1 and 1 is clipped out? or should be clipped out, but we don't do it?
                // Somehow offscreen vertices are being clipped out at a later process - likely at the bounds checking during rasterization
                
                vec4 viewport_coord = (viewport * screen);
                // Need to convert to int otherwise weird black gaps will appear between triangles
                viewport_coord.x = (int) viewport_coord.x;
                viewport_coord.y = (int) viewport_coord.y;
                viewport_coord.w = w;
                viewport_coords.push_back(viewport_coord);

                // We can obtain the vertex index by calling idx.normal_index.
                // All normals are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index + 2];

                vec4 normal = vec4(nx, ny, nz, 0);
                // When transforming vertices, we also need to transform normals, by the transpose inverse of the matrix we use.
                // We only use the view * model matrix to do this however
                // https://graphics.stanford.edu/courses/cs248-05/persp/persp2.html
                normal = mv_transpose_inv * normal;

                normals.push_back(vec3(normal));

                // Textures.
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];
                texture_coords.push_back(vec2(tx, ty));
            }

            // Calculate the normal of the face.
            vec3 normal = cross(world_coords[1] - world_coords[0], world_coords[2] - world_coords[0]);
            normal.normalize_inplace();

            // Create the viewing vector - using one of the vertices of the polygon, create a vector from the eye's position to it.
            // Note that our eye is in world space and we're using the world space version of the vertex.
            vec3 viewing = world_coords[0] - eye;

            float facing = dot(normal, viewing);
            // If facing is positive, that means the polygon is facing the same direction as the viewing vector, aka, AWAY
            if (facing < 0)
            {
                // Calculate the intensities of each vertex by using the normals.
                std::vector<float> intensities;

                // For point light
                // We may need to normalize the world_coords.
                vec3 to_light_v0 = light_dir - world_coords[0];
                vec3 to_light_v1 = light_dir - world_coords[1];
                vec3 to_light_v2 = light_dir - world_coords[2];

                /* to_light_v0.print(); */
                /* to_light_v1.print(); */
                /* to_light_v2.print(); */

                /* normals[0].print(); */
                /* normals[1].print(); */
                /* normals[2].print(); */

                // Set to 0.1f for ambient lighting.
                // We use light_dir for directional light.
                // TODO: do we need to normalize the normals too?
                /* float intensity0 = std::max(dot(normals[0], to_light_v0), 0.1f); */
                /* float intensity1 = std::max(dot(normals[1], to_light_v1), 0.1f); */
                /* float intensity2 = std::max(dot(normals[2], to_light_v2), 0.1f); */ 

                float intensity0 = std::max(dot(normals[0], light_dir), 0.1f);
                float intensity1 = std::max(dot(normals[1], light_dir), 0.1f);
                float intensity2 = std::max(dot(normals[2], light_dir), 0.1f); 

                // Attenuation.
                // This makes the model really dark though. Apparently quadratic attenuation is rarely used so the formula is:
                // 1.0 / (1 + a*d)
                // http://math.hws.edu/graphicsbook/c7/s2.html 7.27
                /* intensity0 = intensity0 * (1 / (1 + (0.5 * dist0))); */
                /* intensity1 = intensity1 * (1 / (1 + (0.5 * dist1))); */
                /* intensity2 = intensity2 * (1 / (1 + (0.5 * dist2))); */

                intensities.push_back(intensity0);
                intensities.push_back(intensity1);
                intensities.push_back(intensity2);

                float intensity = std::max(dot(normal, to_light_v0), 0.0f);
                uint32_t color = SDL_MapRGBA(pixel_format, intensity * 255, intensity * 255, intensity * 255, 255);
                
                // draw_line(viewport_coords[0], viewport_coords[1], viewport_coords[2], frame);
                draw_triangle(viewport_coords[0], viewport_coords[1], viewport_coords[2], frame, z_buffer, texture_coords, texture, intensities, color, colors, normals, eye, light_dir, clip_xy);
            }   
           
            // The index at which each face begins in mesh.indices.
            // Remember that each face could have a variable number of vertices (3, 4, 5, etc.), so the index at which a face begins must be adjusted
            // each time for each face that we process.
            f_index_begin += num_vertices;
        }
    }
    #endif

    frame.flip_image_on_x_axis();
    
    // Main loop.
    while (!quit)
    {
        // Event loop.
        while (SDL_PollEvent(&event) != 0)
        {
            quit = (event.type == SDL_QUIT);
        }

        // Replace the screen with our frame buffer.
        // The last argument is the number of bytes between one row and the next.
        SDL_UpdateTexture(screen, NULL, frame.buffer, WINDOW_WIDTH * sizeof(uint32_t));

        // Render.
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

//    delete z_buffer; // deallocate z-buffer
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
