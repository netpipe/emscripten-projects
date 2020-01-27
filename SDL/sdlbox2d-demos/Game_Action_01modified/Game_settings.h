#ifndef GAME_SETTINGS_H_INCLUDED
#define GAME_SETTINGS_H_INCLUDED

#include "header.h"

struct  GameSeting {// ������� ���������
	int         GameStat = 0;   // ����� � ���� 0 ���� 1,2 ����
	bool        Sinhr = 1;   // ����� � ���� 0 ���� 1,2 ����
	bool        GameHod = 0;        // ��� ��� � ������ ������
	int         Player = 1;     // ������� ����� ������ 1 - ����� � �����������  2 ����� � �������
	int         LastS;          // ��������� ��� ������
	int         LastN;          // ��������� ��� �����
	int         width = 1280;  // ����������
	int         height = 768;   //
	int         FullScr = 0; // �� ���� ����� 11 ���� 12

	int         lvlComlite = 193;
	float       CelSelTime;

	int         lvl = 0;        // ����� ���������� ������ ������� � 1 ��
	int         Sound = 1;      // ����   5 ��� 6 ����
	int         Music = 1;      // ������ 7 ��� 8 ����

} GS;

 int InitWindow()
 {
    //������ ���� �������
    SDL_Surface *surface = IMG_Load("./media/cursor.png");
    SDL_Cursor  *cursor =  SDL_CreateColorCursor(surface, 17, 17);
    SDL_SetCursor   (cursor);
    if (surface) {SDL_FreeSurface(surface);}

    // ��������� ����
    Uint32 flagsW = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;  //N || SDL_WINDOW_RESIZABLE
    if (GS.FullScr) flagsW = flagsW |  SDL_WINDOW_FULLSCREEN;

    window = SDL_CreateWindow("OpenIT SDL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GS.width, GS.height, flagsW );
                if (window == 0){std::cout << SDL_GetError() << std::endl; return 2;}

    Uint32 flagsR = SDL_RENDERER_ACCELERATED;
    if (GS.Sinhr) flagsR = flagsR | SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, flagsR);   //
                if (renderer == 0){std::cout << SDL_GetError() << std::endl;return 3;}

     SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "10"); //��� ����������� �������
 }

int InitMedia()
 {
     Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 );
 }


//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN );


#endif // GAME_SETTINGS_H_INCLUDED
