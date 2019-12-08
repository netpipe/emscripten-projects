#include <stdio.h>
#include <SDL2/SDL.h>
#include <assert.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

int main(int argc, char* argv[])
{
    // setup
    SDL_Window * window;
    SDL_Renderer * renderer;
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);
    SDL_CreateWindowAndRenderer(640, 320, 0, &window, &renderer);

    // clear screen with red color
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    SDL_RenderClear(renderer);

    // draw green line across screen
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderDrawLine(renderer, 0, 0, 640, 320);
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);

    // free resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
