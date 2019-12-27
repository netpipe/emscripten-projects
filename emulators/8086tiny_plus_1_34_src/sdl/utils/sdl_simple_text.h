// =============================================================================
// File: sdl_simple_text.h
//
// Description:
// SDL Simple text output functions.
// Draws text in a fixed with, fixed height font.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//


#ifndef __SDL_SIMPLE_TEXT_H
#define __SDL_SIMPLE_TEXT_H

#include <SDL2/SDL.h>

// Function: SDLTEXT_DrawString
//
// Description:
// Draw a text string on a surface.
//
// Parameters:
//
//   Surface : The surface to be drawn upon
//
//   x : The left edge of the text
//
//   y : The top of the text
//
//   C : The colour
//
//   Str : The null terminated string
//
// Returns:
//
//   int : The width of the text drawn in pixels
//
int SDLTEXT_DrawString(SDL_Surface *Surface, int x, int y, unsigned int C, const char *Str);

void SDLTEXT_GetSize(const char *Str, int &sx, int &sy, int len);

#endif // __SDL_SIMPLE_TEXT_H
