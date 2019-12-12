#include <iostream>
#include <utility>
#include <cmath>

#include "SDL.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "frame.h"

const inline int WINDOW_WIDTH = 640;
const inline int WINDOW_HEIGHT = 480;

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
            frame.set_pixel(x, y, 0xffff0000);
        }
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

    // Load our object.
    // monkey_flat.obj only specifies vertices, normals and faces.
    std::string inputfile = "obj/monkey_flat.obj";
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

    // Time to draw the mesh. It's upside down b/c right now our coordinate system is reflected across the x-axis due to the way the buffer is set-up.
    // For each shape...
    for (size_t s = 0; s < shapes.size(); s++)
    {
        // For each face...
        size_t f_index_begin = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
        {
            // Find the number of vertices that describes it. 
            int num_vertices = shapes[s].mesh.num_face_vertices[f];
            
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

                // We draw a line between this vertex and the next vertex.
                // Note that all vertices are normalized. We add +1 to make sure all our coordinates are positive, and then shift it to w/2 and h/2.
                // This is b/c we have no projection, but we just want to test the rendering.
                // Inspired by tinyrenderer.
                draw_line((vx+1)*frame.w/2, (vy+1)*frame.h/2, (next_vx+1)*frame.w/2, (next_vy+1)*frame.h/2, frame);

                // We can obtain the vertex index by calling idx.normal_index.
                // All normals are listed in a linear array, so our stride is 3 (x,y,z)
                tinyobj::real_t nx = attrib.vertices[3*idx.normal_index + 0];
                tinyobj::real_t ny = attrib.vertices[3*idx.normal_index + 1];
                tinyobj::real_t nz = attrib.vertices[3*idx.normal_index + 2];
            }
           
            // The index at which each face begins in mesh.indices.
            // Remember that each face could have a variable number of vertices (3, 4, 5, etc.), so the index at which a face begins must be adjusted
            // each time for each face that we process.
            f_index_begin += num_vertices;
        }
    }
    
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

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
