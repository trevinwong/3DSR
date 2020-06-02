#include <iostream>
#include <memory>
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
#include "renderer.h"

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

    SDL_Renderer* screen_renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Texture* screen = SDL_CreateTexture(screen_renderer,
                          SDL_PIXELFORMAT_ARGB8888,
                          SDL_TEXTUREACCESS_STREAMING,
                          WINDOW_WIDTH, WINDOW_HEIGHT);
    
    Frame frame(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_AllocFormat(SDL_PIXELFORMAT_ARGB8888));

    auto texture = std::make_shared<Texture>("img/african_head_diffuse.tga"); 

    // TODO: convert everything to smart pointers
    Mesh mesh("obj/african_head.obj", texture);

    Object head(&mesh);

    World world;
    world.addObject(&head);

    // DEPRECATED, should be replaced with Object abstraction model
    mat4 model = makeTranslation(0, 0, 0);
    world.add_mesh_to_world(mesh, model);
    world.set_light(vec3(0, 0, 3));
    world.set_eye(vec3(0, 1, 3));
    world.set_look_at_pt(vec3(0,0,0));

    Renderer framebuffer_renderer(world, frame);

    double eye_angle = M_PI/2;
    double light_angle = M_PI/2;
    double ROT_SPEED = 0.1f;
    double DISTANCE = 2;
    double TWO_PI = 2.0*M_PI;
    
    while (!quit)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            switch (event.type)
            {
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym )
                {
                case SDLK_LEFT:
                    eye_angle = std::fmod(eye_angle + ROT_SPEED, TWO_PI);
                    break;
                case SDLK_RIGHT:
                    eye_angle = std::fmod(eye_angle - ROT_SPEED, TWO_PI);
                    break;
                case SDLK_a:
                    light_angle = std::fmod(light_angle + ROT_SPEED, TWO_PI);
                    break;
                case SDLK_d:
                    light_angle = std::fmod(light_angle - ROT_SPEED, TWO_PI);
                    break;
                default:
                    break;
                }
            case SDL_KEYUP:
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            default:
                break;
            }
        }

        // TODO: Abstract rotation
        world.set_eye(vec3(cos(eye_angle) * DISTANCE, 1, sin(eye_angle) * DISTANCE));
        world.set_light(vec3(cos(light_angle) * DISTANCE, 1, sin(light_angle) * DISTANCE));

        framebuffer_renderer.render();
        frame.flip_image_on_x_axis();

        SDL_UpdateTexture(screen, NULL, frame.buffer, WINDOW_WIDTH * sizeof(uint32_t));
        
        SDL_RenderClear(screen_renderer);
        SDL_RenderCopy(screen_renderer, screen, NULL, NULL);
        SDL_RenderPresent(screen_renderer);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
