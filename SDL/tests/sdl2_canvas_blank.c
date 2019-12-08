/*
 * Copyright 2014 The Emscripten Authors.  All rights reserved.
 * Emscripten is available under two separate licenses, the MIT license and the
 * University of Illinois/NCSA Open Source License.  Both these licenses can be
 * found in the LICENSE file.
 */

#include <stdio.h>
#include <SDL2/SDL.h>

//#ifdef __EMSCRIPTEN__
//#include <emscripten.h>
//#endif

void main_loop(){

}

int main() {
  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_CreateWindowAndRenderer(256, 256, 0, &window, &renderer);

  SDL_Surface *screen = SDL_CreateRGBSurface(0, 256, 256, 8, 0, 0, 0, 0);

  if (SDL_MUSTLOCK(screen)) SDL_LockSurface(screen);
  if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
  SDL_RenderPresent(renderer);
//  #ifdef __EMSCRIPTEN__
//	emscripten_set_main_loop(main_loop,0,1);
//#else
//
//
//#endif

  // Don't quit - we need to reftest the canvas! SDL_Quit();

  return 0;
}

