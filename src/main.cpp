#include <iostream>

#include "SDL.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

const inline int WINDOW_WIDTH = 640;
const inline int WINDOW_HEIGHT = 480;

int main() {
    SDL_Window* window = nullptr;
    SDL_Surface* screenSurface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError()); 
    } else
    {
        window = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

        if (!window)
        {
            printf("SDL_Window could not be created. SDL_Error: %s\n", SDL_GetError()); 
        } else
        {
            screenSurface = SDL_GetWindowSurface(window);
            SDL_FillRect(screenSurface, NULL, SDL_MapRGB(screenSurface->format, 0xFF, 0xFF, 0xFF));
            SDL_UpdateWindowSurface(window);
            SDL_Delay(2000);

            SDL_DestroyWindow(window);
            SDL_Quit();
        }
    }
}
