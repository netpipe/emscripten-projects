/*
 * main.c
 *
 *  Created on: 10 jan 2018
 *      Author: Carlos Faruolo
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "custom_mix_pitch.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

Mix_Chunk* chunk;
float speed;

void main_loop(){

	if(chunk != NULL)
	{
		// play the chunk
		const int channel = Mix_PlayChannel(-1, chunk, -1);

		// set pitch effect on the channel where the chunk is being played
		Custom_Mix_RegisterPlaybackSpeedEffect(channel, chunk, &speed, 1);

		const int secs = 1;
		printf("Looping for %d seconds, changing the pitch dynamically...\n", secs);

		// loop for 'secs' seconds, changing the pitch dynamically
		while(SDL_GetTicks() < secs*1000)
		{
			speed = 1 + 0.25*sin(0.001*SDL_GetTicks());
		}
		puts("Finished.");
	}

	}
// example
// run the executable passing an filename of a sound file that SDL_mixer is able to open (ogg, wav, ...)
int main(int argc, char** argv)
{
//	if(argc < 2) { puts("Missing argument."); return 0; }

	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
	Mix_AllocateChannels(MIX_CHANNELS);

	speed = 1.0;
	chunk = Mix_LoadWAV("./media/bling.wav");

	#ifdef __EMSCRIPTEN__
		emscripten_set_main_loop(main_loop,0,1);
	#else


	if(chunk != NULL)
	{
		// play the chunk
		const int channel = Mix_PlayChannel(-1, chunk, -1);

		// set pitch effect on the channel where the chunk is being played
		Custom_Mix_RegisterPlaybackSpeedEffect(channel, chunk, &speed, 1);

		const int secs = 8;
		printf("Looping for %d seconds, changing the pitch dynamically...\n", secs);

		// loop for 'secs' seconds, changing the pitch dynamically
		while(SDL_GetTicks() < secs*1000)
		{
			speed = 1 + 0.25*sin(0.001*SDL_GetTicks());
		}
		puts("Finished.");
		//SDL_Delay(5);
	}
	else
		puts("No data.");
	#endif
	Mix_FreeChunk(chunk);
	Mix_CloseAudio();
	Mix_Quit();
	SDL_Quit();
}
