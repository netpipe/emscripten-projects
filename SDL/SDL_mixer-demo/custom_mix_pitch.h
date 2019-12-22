/*
 * custom_mix_pitch.h
 *
 *  Created on: 10 jan 2018
 *      Author: Carlos Faruolo
 */

#ifndef CUSTOM_MIX_PITCH_H_
#define CUSTOM_MIX_PITCH_H_

#include <SDL2/SDL_mixer.h>

// Register a proper playback speed effect handler for this channel according to the current audio format. Effect valid for the current (or next) playback only.
void Custom_Mix_RegisterPlaybackSpeedEffect(int channel, Mix_Chunk* chunk, float* speed, int loop);

#endif /* CUSTOM_MIX_PITCH_H_ */
