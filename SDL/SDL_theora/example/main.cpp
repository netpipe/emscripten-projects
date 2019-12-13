#include <SDL_theora.h>

#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <cstdlib>

using namespace std;

//just a quick test to see.


int my_video;
struct context
{
    SDL_Renderer *renderer;
    int iteration;
};

void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;

    // example: draw a moving rectangle

    // red background
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    // moving blue rectangle
    SDL_Rect r;
    r.x = ctx->iteration % 255;
    r.y = 50;
    r.w = 50;
    r.h = 50;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255 );
    SDL_RenderFillRect(renderer, &r );

    SDL_RenderPresent(renderer);

if(THR_IsPlaying(my_video) == 0)
	THR_DestroyVideo(my_video);


    ctx->iteration++;
}

int main()
{
//    std::cout << "test";
    printf("initializing SDL");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(500, 500, 0, &window, &renderer);

//    SDL_Window *window = SDL_CreateWindow("Title", 100, 100, 400, 600,SDL_WINDOW_SHOWN);
//    SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
  //  SDL_Texture* texture = IMG_LoadTexture(renderer, "FILE");

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
printf("loading video");

//THR_Init(1); //testing

 my_video = THR_Load("media/YUVTest.ogg",renderer);
//
//SDL_Texture* video_texture = THR_UpdateVideo(my_video);
// Use SDL_RenderCopy to blit the texture normally
//
//if(THR_IsPlaying(my_video) == 0)
//	THR_DestroyVideo(my_video);


#ifdef __EMSCRIPTEN__
   emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
#endif

    SDL_Rect r;
    r.x = 0;  //the x coordinate
r.y = 0; // the y coordinate
r.w = 500; //the width of the texture
r.h = 500; //the height of the texture

SDL_Rect texture_rect;
texture_rect.x = 0;  //the x coordinate
texture_rect.y = 0; // the y coordinate
texture_rect.w = 500; //the width of the texture
texture_rect.h = 500; //the height of the texture


while (1) {
//    context *ctx = static_cast<context*>(arg);
//    SDL_Renderer *renderer = ctx->renderer;
printf("rendering");
    // example: draw a moving rectangle

    // red background
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);


//    if(THR_IsPlaying(my_video) == 0)
//        THR_DestroyVideo(my_video);
//
//  video_texture = THR_UpdateVideo(my_video);
//
//    // moving blue rectangle


    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255 );
    SDL_RenderFillRect(renderer, &r );
//
//SDL_RenderCopy(renderer, video_texture, NULL, &texture_rect);


    SDL_RenderPresent(renderer);


  ctx.iteration++;

}


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

