#ifndef GAME_SETTINGS_H_INCLUDED
#define GAME_SETTINGS_H_INCLUDED

#include "header.h"


struct  GameSeting {// игровые настройки
	int         GameStat = 0;   // режим в игре 0 меню 1,2 игра
	bool        Sinhr = 1;   // режим в игре 0 меню 1,2 игра
	bool        GameHod = 0;        // чей ход в данный момент
	int         Player = 1;     // сколько игров играет 1 - игрок с компьютером  2 игрок с игроком
	int         LastS;          // последний ход строка
	int         LastN;          // последний ход номер
	int         width = 1280;  // разрешение
	int         height = 640;   //
	int         FullScr = 0; // на весь экран 11 окно 12

	int         lvlComlite = 193;
	float       CelSelTime;

	int         lvl = 0;        // номер выбранного уровн€ начина€ с 1 до
	int         Sound = 1;      // звук   5 вкл 6 выкл
	int         Music = 1;      // музыка 7 вкл 8 выкл

} GS;

SDL_Rect camera = {0,0,GS.width,GS.height};

void setPlayerCoordinateForView(float x, float y) {


	camera.x =  x - GS.width / 2;
	camera.y =  y - GS.height / 2;
	if( camera.x < 0 )
            camera.x = 0;
	if( camera.y < 0 )
            camera.y = 0;
    if( camera.x > 1600 - camera.w )
            camera.x = 1600 - camera.w;
    if( camera.y > GS.height - camera.h )
			camera.y = GS.height - camera.h;
}


 InitWindow()
 {
    //замена мыши курсора
    SDL_Surface *surface = IMG_Load("cursor.png");
    SDL_Cursor  *cursor =  SDL_CreateColorCursor(surface, 17, 17);
    SDL_SetCursor   (cursor);
    if (surface) {SDL_FreeSurface(surface);}

    // настройка окна
    Uint32 flagsW = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;  //N || SDL_WINDOW_RESIZABLE
    if (GS.FullScr) flagsW = flagsW |  SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow("OpenIT SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GS.width, GS.height, flagsW );
                if (window == 0){std::cout << SDL_GetError() << std::endl; return 2;}

    Uint32 flagsR = SDL_RENDERER_ACCELERATED;
    if (GS.Sinhr) flagsR = flagsR | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, flagsR);   //
                if (renderer == 0){std::cout << SDL_GetError() << std::endl;return 3;}

     SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "10"); //ƒл€ сглаживани€ текстур
 }

 InitMedia()
 {
     Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
 }


//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN );


#endif // GAME_SETTINGS_H_INCLUDED
