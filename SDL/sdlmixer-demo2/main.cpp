#include "SDL2SoundEffects.h"
#include <iostream>
#include <SDL2/SDL.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

SDL2SoundEffects se;

void main_loop(){
    se.playSoundEffect(0);
    SDL_Delay(3500);
  }

int main()
{
  se.addSoundEffect("./media/bling.wav");
  #ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(main_loop,0,1);
	#endif

  return 0;
}
