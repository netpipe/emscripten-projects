
#ifndef SDL_GAME_H_INCLUDED
#define SDL_GAME_H_INCLUDED

#include "header.h"

// Класс с функциями для объектов мира
class SDL_game
{
private:

    float scaleH = 1;
    float scaleW = 1;
    float angle =  0;
    SDL_Point pointCenter={0,0};
    SDL_Rect rectWin = {0,0,0,0};;
    SDL_Rect rectSpr;
    SDL_Color color = {255,255,255,255};
    int wText=0;
    int hText=0;


public:
SDL_Texture *text = NULL;


int setTexture  (SDL_Texture *t)
        {
         text = t;
         SDL_QueryTexture(text, NULL, NULL, &wText, &hText);
         rectSpr.x =0;
         rectSpr.y =0;
         rectSpr.w = wText;
         rectSpr.h = hText;
        }

int setTextureFile (std::string file)
{
   SDL_Surface *loadedImage = 0;
   text = 0;
   loadedImage = IMG_Load (file.c_str());   //SDL_LoadBMP
   if (loadedImage){
       text = SDL_CreateTextureFromSurface(renderer, loadedImage);
       SDL_FreeSurface(loadedImage);
   }
   else
       std::cout << SDL_GetError() << std::endl;
   SDL_QueryTexture(text, NULL, NULL, &wText, &hText);
   rectSpr.x =0;
   rectSpr.y =0;
   rectSpr.w = wText;
   rectSpr.h = hText;

}



SDL_Texture* loadImg(std::string file)
{
   SDL_Surface *loadedImage = 0;

   loadedImage = IMG_Load (file.c_str());   //SDL_LoadBMP
   if (loadedImage){
       text = SDL_CreateTextureFromSurface(renderer, loadedImage);
       SDL_FreeSurface(loadedImage);
   }
   else
       std::cout << SDL_GetError() << std::endl;
   return text;
}



void setPosition (float x, float y)

        {rectWin.x = x; rectWin.y=y;}

void setRotation (float a)

        {angle=a;}

void setTextureRect(SDL_Rect r)

        {rectSpr = r;}

void setTextureRect(int x,int y,int w,int h)
        {
         rectSpr.x =x;
         rectSpr.y =y;
         rectSpr.w =w;
         rectSpr.h =h;
        }

void setColor (int r, int g, int b, int a)

        {
        color.r=r;
        color.g=g;
        color.b=b;
        color.a=a;
        SDL_SetTextureColorMod  (text, color.r, color.g, color.b );
        SDL_SetTextureAlphaMod  (text, color.a);
        }

void setScale (float w, float h)
        {
         scaleW =  w;
         scaleH =  h;
        }

void setOrigin (int x, int y)
        {
          pointCenter.x = x;
          pointCenter.y = y;

        }

void Draw (SDL_Renderer *rend, SDL_Texture *texture)
        {
        SDL_Point Centr;
        SDL_Rect DrawWin;
        Centr.x = pointCenter.x*scaleW;
        Centr.y = pointCenter.y*scaleW;
        rectWin.w=rectSpr.w*scaleW;
        rectWin.h=rectSpr.h*scaleH;
        DrawWin = rectWin;
        DrawWin.x -=    Centr.x;
        DrawWin.y -=    Centr.y;
        SDL_RenderCopyEx        (rend, texture, &rectSpr, &DrawWin, angle , &Centr ,SDL_FLIP_NONE);
        }

void         Draw (SDL_Renderer *rend)
        {
        SDL_Point Centr;
        SDL_Rect DrawWin;
        Centr.x = pointCenter.x*scaleW;
        Centr.y = pointCenter.y*scaleW;
        rectWin.w=rectSpr.w*scaleW;
        rectWin.h=rectSpr.h*scaleH;
        DrawWin = rectWin;
        DrawWin.x -=    Centr.x;
        DrawWin.y -=    Centr.y;
        SDL_RenderCopyEx        (rend, text, &rectSpr, &DrawWin, angle , &Centr ,SDL_FLIP_NONE);
        }
};
// конец - Класс с функциями для объектов мира


// Таймер и FPS
class Timer
{
private:
    int     lasttime=0;
    int     lasttimefps=0;
    int     FPScount=0;

public:
    int     FPS=0;

    void    Reset()
            {
                lasttime = SDL_GetTicks();
            }
    float   GetTimer()
            {
                return SDL_GetTicks() - lasttime;
            }

    int     GetFPS()
            {
                FPScount++;
             if (SDL_GetTicks() - lasttimefps >= 1000)
                {
                    FPS = FPScount;  lasttimefps = SDL_GetTicks(); FPScount=0;
                }
            return FPS;
            }
};
// конец - Таймер и FPS





#endif // SDL_GAME_H_INCLUDED
