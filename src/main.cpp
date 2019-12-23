#include <iostream>
#include <limits>
#include <utility>
#include <cmath>
#include <iomanip>
#include <cstdint>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../lib/tiny_obj_loader.h"

#include "vec2.h"
#include "vec3.h"
#include "frame.h"

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

// We are given two vectors with z = 0, and want to calculate their cross product.
// This is equivalent to the 2x2 determinant formed by the vectors created by their x and y components.
// This is also equivalent to the area of the parallelogram formed by the two vectors.
// This is also equivalent to comparing b to the edge function of a.
float cross_2d(const vec2& a, const vec2& b)
{
    return (a.x * b.y) - (a.y * b.x);
}

int min3(int a, int b, int c)
{
    return std::min(std::min(a, b), c);
}

int max3(int a, int b, int c)
{
    return std::max(std::max(a, b), c);
}

void print_hex(uint32_t num)
{
    std::cout << std::setfill('0') << std::setw(8) << std::hex << num << '\n';
}

// Assume p0, p1 and p2 are listed in CCW order, and we're using a right-hand coordinate system.
void draw_triangle(vec3& p0, vec3& p1, vec3& p2, Frame& frame, uint32_t color, float* z_buffer)
{
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
    for (int i = min_x; i < max_x; i++)
    {
        for (int j = min_y; j < max_y; j++)
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

                // Calculate barycentric coordinates. We can leave out the division by 2 since we're just getting the ratio between the sub-triangle and the triangle.
                float w0 = v0v1p / v0v1v2; // corresponds to the weight of p2
                float w1 = v1v2p / v0v1v2; // corresponds to the weight of p0
                float w2 = v2v0p / v0v1v2; // corresponds to the weight of p1

                float z = 0;
                z += (w0 * p2.z);
                z += (w1 * p0.z);
                z += (w2 * p1.z);


                // If the z-value of the pixel we're on is greater than our current stored z-buffer's value...
                if (z_buffer[i + (j * frame.w)] < z)
                {
                    // Replace it and color that pixel in.
                    z_buffer[i + (j * frame.w)] = z;
                    frame.set_pixel(i, j, color);
                }
            }
        }
    }
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
    
    /*
    // 8th quadrant
    draw_line(200, 200, 250, 300, frame);
    // 7th quadrant
    draw_line(200, 200, 300, 250, frame);
    // 1st quadrant
    draw_line(200, 200, 300, 150, frame);
    // 2nd quadrant
    draw_line(200, 200, 250, 100, frame);
    // 3rd quadrant
    draw_line(200, 200, 150, 100, frame);
    // 4th quadrant
    draw_line(200, 200, 100, 150, frame);
    // 5th quadrant
    draw_line(200, 200, 100, 250, frame);
    // 6th quadrant
    draw_line(200, 200, 150, 300, frame);
    */

    /*
    vec2 v0(200, 200);
    vec2 v1(400, 200);
    vec2 v2(300, 300);
    draw_triangle(v0, v1, v2, frame);
    */

    // Load our object.
    // monkey_flat.obj only specifies vertices, normals and faces.
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

    if (!ret) {
      exit(1);
    }

    // "Create" a light, in world coords.
    vec3 light_dir(0, 0, 1);

    // Time to draw the mesh.
   
    // Let's allocate our z-buffer.
    float* z_buffer = new float[frame.w * frame.h];
    for (int i = 0; i < frame.w * frame.h; i++)
    {
        z_buffer[i] = std::numeric_limits<float>::lowest();
    }

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
            std::vector<vec3> screen_coords;
            
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

                tinyobj::real_t next_vx = attrib.vertices[3*next_idx.vertex_index + 0];
                tinyobj::real_t next_vy = attrib.vertices[3*next_idx.vertex_index + 1];
                tinyobj::real_t next_vz = attrib.vertices[3*next_idx.vertex_index + 2];

                world_coords.push_back(vec3(vx, vy, vz));
                screen_coords.push_back(vec3((int) ((vx+1)*frame.w/2), (int)((vy+1)*frame.h/2), vz));

                // We draw a line between this vertex and the next vertex.
                // Note that all vertices are normalized, and in "world coordinates".
                // We scale it to screen coordinates and do what seems like "orthogonal projection" by completely ignoring z.
                // Inspired by tinyrenderer.
                // draw_line((vx+1)*frame.w/2, (vy+1)*frame.h/2, (next_vx+1)*frame.w/2, (next_vy+1)*frame.h/2, frame);

                // We can obtain the vertex index by calling idx.normal_index.
                // All normals are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t nx = attrib.vertices[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.vertices[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.vertices[3*idx.normal_index + 2];
            }

            // The face normal is given, but let's calculate them for fun.
            vec3 normal = cross(world_coords[1] - world_coords[0], world_coords[2] - world_coords[0]);
            normal.normalize_inplace(); 
            float intensity = dot(normal, light_dir);
            if (intensity > 0.0f)
            {
                draw_triangle(screen_coords[0], screen_coords[1], screen_coords[2], frame, SDL_MapRGBA(pixel_format, intensity*255, intensity*255, intensity*255, 255), z_buffer);
            }
           
            // The index at which each face begins in mesh.indices.
            // Remember that each face could have a variable number of vertices (3, 4, 5, etc.), so the index at which a face begins must be adjusted
            // each time for each face that we process.
            f_index_begin += num_vertices;
        }
    }

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

    delete z_buffer; // deallocate z-buffer
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
