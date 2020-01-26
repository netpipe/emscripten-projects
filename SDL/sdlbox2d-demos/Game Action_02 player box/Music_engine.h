#ifndef MUSIC_ENGINE_H_INCLUDED
#define MUSIC_ENGINE_H_INCLUDED

#include "header.h"

    Mix_Music *music= NULL;
    Mix_Chunk *Fly_SB = NULL;
    Mix_Chunk *Sel_SB = NULL;
    Mix_Chunk *FireWAV = NULL;

    InitMusic() {


    Mix_AllocateChannels(1);
    music = Mix_LoadMUS("musicgameplay.ogg");
        if(!music) printf("musicgameplay1.ogg eror", Mix_GetError());
    Mix_PlayMusic(music, -1);
    FireWAV = Mix_LoadWAV("fire.wav");

    Fly_SB = Mix_LoadWAV("fire.wav");
    Sel_SB = Mix_LoadWAV("res/Sound/select.wav");


    }
#endif // MUSIC_ENGINE_H_INCLUDED
