#include <iostream>
#include <limits>
#include <utility>
#include <cmath>

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "mat4.h"
#include "frame.h"
#include "object.h"
#include "texture.h"
#include "utils.h"
#include "mesh.h"
#include "world.h"
#include "rasterizer.h"

#define trace(var)  { std::cout << "Line " << __LINE__ << ": " << #var << "=" << var << "\n";}

#include "SDL.h"

const inline int WINDOW_WIDTH = 800;
const inline int WINDOW_HEIGHT = 800;

bool quit = false;

int main() {
    SDL_Event event;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return 1;

    SDL_Window* window = SDL_CreateWindow("3DSR",
                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                    WINDOW_WIDTH, WINDOW_HEIGHT,
                    SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture* screen = SDL_CreateTexture(renderer,
                          SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING,
                          WINDOW_WIDTH, WINDOW_HEIGHT);
    
    Frame frame(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888));

    Texture texture("img/african_head_diffuse.tga"); 
    Mesh mesh("obj/african_head.obj", &texture);

    Object head(&mesh);

    // TO-DO: Remove this matrix. The Object "head" should be added to the world instead of the matrix itself being added raw.
    mat4 model      (   1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1
                    );

    World world;
    world.add_mesh_to_world(mesh, model);
    // Should not be normalized, technically, if we wanna do attenuation
    world.set_light(vec3(0.5, -1, -1).normalize());
    world.set_eye(vec3(1, 1, 3));

    Rasterizer rasterizer(world, frame);

    double angle = 0;
    double TWO_PI = 2.0*M_PI;
    
    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            quit = (event.type == SDL_QUIT);
        }

        angle = std::fmod(angle + 0.1f, TWO_PI);
        world.set_eye(vec3(cos(angle) * 3, 1, sin(angle) * 3));
        rasterizer.render();
        frame.flip_image_on_x_axis();

        // Replace the screen with our frame buffer.
        // The last argument is the number of bytes between one row and the next.
        SDL_UpdateTexture(screen, NULL, frame.buffer, WINDOW_WIDTH * sizeof(uint32_t));

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, screen, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

//    delete z_buffer; // deallocate z-buffer
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
