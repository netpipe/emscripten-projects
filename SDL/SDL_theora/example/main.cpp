#include "SDL_theora.h"




---
Basic use example:

int my_video = THR_Load("data/intro.ogv");

SDL_Texture* video_texture = THR_Update(my_video);
// Use SDL_RenderCopy to blit the texture normally

if(THR_IsPlaying(my_video) == 0)
	THR_DestroyVideo(my_video);
