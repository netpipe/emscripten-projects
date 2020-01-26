#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED
#include <cmath>
#include <list>
#include <vector>
#include <Box2D/Box2D.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <iostream>
#include <string.h>
#include <string>
#include <clocale>      //для русских символов в консоле
#include <stdio.h>
#include <sstream>

// Глобальные переменные для SDL
SDL_Window      *window = NULL;
SDL_Renderer    *renderer = NULL;


// Глобальные переменные для Box2D
const float SCALE = 30.f;
const float DEG  =  57.29577f;


b2Vec2 Gravity(0.f, 9.8f);

b2World World( Gravity);


#endif // HEADER_H_INCLUDED
