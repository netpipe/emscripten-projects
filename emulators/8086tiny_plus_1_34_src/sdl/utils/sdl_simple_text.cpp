
#include "sdl_simple_text.h"
#include "vga_glyphs.h"


int SDLTEXT_DrawString(SDL_Surface *Surface, int x, int y, unsigned int C, const char *Str)
{

  unsigned char *bm;
  unsigned char *gd;
  unsigned char *ci;
  int Pitch = Surface->pitch;
  int BPP   = Surface->format->BytesPerPixel;
  int gx, gy;
  int c_left, c_right;
  unsigned char Mask;
  int StrWidth = 0;
  const unsigned char *UStr = (const unsigned char *) Str;
  bool NoDraw = false;

  if (SDL_MUSTLOCK(Surface))
  {
    SDL_LockSurface(Surface);
  }

  SDL_Rect ClipRect;
  SDL_GetClipRect(Surface, &ClipRect);

  // Check if any of the text is within the clip rect
  if (x >= ClipRect.x + ClipRect.w) NoDraw = true;
  if (y >= ClipRect.y + ClipRect.h) NoDraw = true;
  if (((int) (x+8*strlen(Str))) < ClipRect.x) NoDraw = true;
  if ((y+16) < ClipRect.y) NoDraw = true;

  if (NoDraw)
  {
    if (SDL_MUSTLOCK(Surface))
    {
      SDL_UnlockSurface(Surface);
    }

    return 0;
  }

  for (int i = 0 ; Str[i] != 0 ; i++)
  {
    c_left = ClipRect.x - (x+i*8);
    if (c_left > 7) c_left = 8;
    if (c_left < 0) c_left = 0;

    c_right = (ClipRect.x + ClipRect.w) - (x+i*8);
    if (c_right > 8) c_right = 8;
    if (c_right < 0) c_right = 0;

    bm = ((unsigned char *) Surface->pixels) + ((y * Pitch) + (x + i * 8 + c_left) * BPP);


    gd = VGAGlyphs + (UStr[i] << 4);

    for (gy = 0 ; gy < 16 ; gy++)
    {
      if ((y+gy >= ClipRect.y) && (y+gy < ClipRect.y + ClipRect.h))
      {
        Mask = 0x80 >> c_left;
        for (gx = c_left ; gx < c_right ; gx++)
        {
          if ((*gd) & Mask)
          {
            ci = (unsigned char *) &C;
            *bm++ = *ci++;
            *bm++ = *ci++;
            *bm++ = *ci++;
            if (BPP > 3) bm++;
          }
          else
          {
            bm += BPP;
          }

          Mask >>= 1;
        }
      }
      else
      {
        bm += BPP * 8;
      }

      bm += Pitch - 8*BPP;
      gd++;
    }

    StrWidth += 8;
  }

  if (SDL_MUSTLOCK(Surface))
  {
    SDL_UnlockSurface(Surface);
  }

  return StrWidth;
}

void SDLTEXT_GetSize(const char *Str, int &sx, int &sy, int len)
{
  sy = 16;
  sx = len * 8;
}


