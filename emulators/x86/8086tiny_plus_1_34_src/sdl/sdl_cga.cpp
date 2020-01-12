// =============================================================================
// File: sdl_cga.cpp
//
// Description:
// SDL implementation of MCGA emulation.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include <stdio.h>

#include "sdl_cga.h"
#include "cga_glyphs.h"
#include "vga_glyphs.h"

enum ScreenMode_t
{
  SM_BW40,
  SM_CO40,
  SM_BW80,
  SM_CO80,
  SM_CO320,
  SM_BW320,
  SM_640x200,
  SM_MODE11,
  SM_MODE13
};

static SDL_Surface *GFX320Surface = NULL;
static SDL_Surface *GFX640Surface = NULL;
static SDL_Surface *GFX640x480Surface = NULL;
static int BPP = 32;
static int BytesPerPixel = 4;
static int DrawTop = 0;

static TextDisplay_t TextDisplay = TD_VGA_8x16;

static unsigned char CGAModeControlRegister = 0;
static unsigned char CGAColourControlRegister = 0;

// CRTC Registers
// Index Port = 0x03d4 (or 0x03b4)
// Data port  = 0x03d5 (or 0x03b5)
static unsigned char CRTIndexRegister = 0;
static unsigned char CRTRegister[16] =
{
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00
};

// Attribute Controller registers
// Port 0x03c0, write index/value
#define AC_REG_COUNT 0x15
static bool ACIndexState = true;
static unsigned char ACIndex = 0;
static const unsigned char DefACRegisters[AC_REG_COUNT] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x0f, 0x08, 0x00
};
static unsigned char ACRegisters[AC_REG_COUNT] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0c, 0x00, 0x0f, 0x08, 0x00
};

// Miscellaneous Output Register
// Write Port 0x03c2
// Read Port  0x03cc
static unsigned char MiscOutputReg = 0x67;

// Colour Read Index Register
// Port = 0x03c7
static unsigned char ColourReadIndex = 0;
static unsigned char ColourReadComponent = 0;
// Colour Write Index Register
// Port = 0x03c8
static unsigned char ColourWriteIndex = 0;
static unsigned char ColourWriteComponent = 0;

// Sequencer registers
// Index Port = 0x03c4
// Data Port  = 0x03c5
//   0 : Reset
//       Bit 0 : Asynchronous reset ( 0 = reset, 1 = run )
//       Bit 1 : Synchronous reset ( 0 = reset & halt, 1 = run )
//   1 : Clocking mode
//       Bit 0 : 8/9 dot clocks ( 0 = 9 dot clocks, 1 = 8 dot clocks )
//       Bit 1 : Bandwidth
//       Bit 2 : Shift load
//       Bit 3 : Dot clock
//       Bit 4-7 : Unused
//   2 : Map mask
//       Bit 0 : Enable map 0
//       Bit 1 : Enable map 1
//       Bit 2 : Enable map 2
//       Bit 3 : Enable map 3
//   3 : Character map select
//   4 : Memory Mode
//       Bit 0 : Alpha-A logical 0 indicates that a non-alpha mode is active.
//               A logical 1 indicates that alpha mode is active and enables
//               the character generator map select function.
//       Bit 1 : Extended Memory-A logical 0 indicates that the memory
//               expansion card is not installed. A logical 1 indicates that
//               the memory expansion card is installed and enables access to
//               the extended memory through address bits 14 and 15.
//       Bit 2 : Odd/Even-A logical 0 directs even processor addresses to
//               access maps 0 and 2, while odd processor addresses access
//               maps 1 and 3. A logical 1 causes processor addresses to
//               sequentially access data within a bit map. The maps are
//               accessed according to the value in the map mask register.
//
#define SQ_REG_COUNT 5
static unsigned char SQIndex = 0;
static const unsigned char DefSQRegisters[SQ_REG_COUNT] =
{
  0x00, 0x01, 0x03, 0x00, 0x07
};
static unsigned char SQRegisters[SQ_REG_COUNT] =
{
  0x00, 0x01, 0x03, 0x00, 0x07
};

// Graphics Controller Registers
// Index Port = 0x03ce
// Data Port  = 0x03cf
//   0 : Set / Reset
//   1 : Enable Set / Reset
//   2 : Colour compare
//   3 : Data rotate
//   4 : Read Map Select
//   5 : Mode Register
//   6 : Miscellaneous
//   7 : Colour Don't Care
//   8 : Bit Mask
#define GC_REG_COUNT 9
static unsigned char GCIndex = 0;
static const unsigned char DefGCRegisters[GC_REG_COUNT] =
{
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x0e, 0x00,
  0xff
};
static unsigned char GCRegisters[GC_REG_COUNT] =
{
  0x00, 0x00, 0x00, 0x00,
  0x00, 0x10, 0x0e, 0x00,
  0xff
};

static int HostOE = 1;
static int WriteMode = 0;
static int ReadMode = 0;
static int LogicOp = 0;
static int RotateCount = 0;
static unsigned char LatchRegisters[4];

static unsigned int PageOffset = 0;
static unsigned int CursorLocation = 0;
static bool  CursorDisplayOn = false;
static unsigned int CursorBlinkTime = 0;
static unsigned char CGAStatus = 0;
static unsigned int CGARetraceEndTime = 0;
static unsigned char CGAPaletteB[16*3] =
{
  0x00, 0x00, 0x00, // black
  0xAA, 0x00, 0x00, // blue
  0x00, 0xAA, 0x00, // green
  0xAA, 0xAA, 0x00, // cyan
  0x00, 0x00, 0xAA, // red
  0xAA, 0x00, 0xAA, // magenta
  0x00, 0x55, 0xAA, // brown
  0xAA, 0xAA, 0xAA, // light gray
  0x55, 0x55, 0x55, // gray
  0xFF, 0x55, 0x55, // light blue
  0x55, 0xFF, 0x55, // light green
  0xFF, 0xFF, 0x55, // light cyan
  0x55, 0x55, 0xFF, // light red
  0xFF, 0x55, 0xFF, // light magenta
  0x55, 0xFF, 0xFF, // light yellow
  0xFF, 0xFF, 0xFF  // white
};

// Palette for 256 colour MCGA mode
static unsigned char MCGAPalette[256*3];

static int CGA320Palette1[4] = { 0, 2, 4, 6 };
static int CGA320Palette2[4] = { 0, 3, 5, 7 };
static int CGA320Palette3[4] = { 0, 10, 12, 14 };
static int CGA320Palette4[4] = { 0, 11, 13, 15 };
static int CGA320Palette5[4] = { 0, 4, 3, 7 };
// static int CGA320Palette6[4] = { 0, 12, 11, 15 };


static int *CGA320Palette = CGA320Palette2;

// The current state drawn for each character cell on the screen
static unsigned char TextState[80*25*2];


static ScreenMode_t CurrentScreenMode = SM_CO80;
static bool ScreenFullRedraw = true;
static int Scaling = 1;

// =============================================================================
// Local Functions
//


static void UpdateCursorstate(void)
{
  int CursorMode = (CRTRegister[0xA] >> 5) & 0x03;
  unsigned int currentTime;

  // Cursor mode:
  //    00b = cursor on
  //    01b = cursor off
  //    10b = cursor blink fast
  //    11b = cursor blink slow

  switch (CursorMode)
  {
    case 0:
      CursorDisplayOn = true;
      break;

    case 1:
      CursorDisplayOn = false;
      break;

    case 2:
      currentTime = SDL_GetTicks();
      if (currentTime > CursorBlinkTime)
      {
        CursorDisplayOn = !CursorDisplayOn;
        CursorBlinkTime = currentTime + 250;
      }
      break;

    case 3:
      currentTime = SDL_GetTicks();
      if (currentTime > CursorBlinkTime)
      {
        CursorDisplayOn = !CursorDisplayOn;
        CursorBlinkTime = currentTime + 500;
      }
      break;
  }
}

static void CGA_DrawCO40(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xb8000 + PageOffset;
  unsigned char *cm = TextState;
  unsigned char *gd;
  unsigned char *bm;
  unsigned char *ci;
  int attr;
  int glyph;
  int gx, gy;
  int fgColour;
  int bgColour;
  unsigned char Mask;

  for (int y = 0 ; y < 25 ; y++)
  {

    for (int x = 0 ; x < 40 ; x++)
    {
      bm = ((unsigned char *) GFX320Surface->pixels) + ((y * 320 * 8 + x * 8) * BytesPerPixel);

      glyph = *(vm++);
      attr = *(vm++);

      if (ScreenFullRedraw || (glyph != cm[0]) || (attr != cm[1]))
      {
        fgColour = attr & 0x0f;
        bgColour = (attr >> 4) & 0x0f;

        gd = CGAGlyphs + (glyph << 3);

        for (gy = 0 ; gy < 8 ; gy++)
        {
          Mask = 0x80;
          for (gx = 0 ; gx < 8 ; gx++)
          {
            if ((*gd) & Mask)
            {
              ci = CGAPaletteB + fgColour*3;
            }
            else
            {
              ci = CGAPaletteB + bgColour*3;
            }

            *bm++ = *ci++;
            *bm++ = *ci++;
            *bm++ = *ci++;
            if (BytesPerPixel > 3) bm++;

            Mask >>= 1;
          }
          bm += 312 * BytesPerPixel;
          gd++;
        }

        cm[0] = glyph;
        cm[1] = attr;
      }
      cm+=2;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 320;
  SRect.h = 200;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX320Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  UpdateCursorstate();

  if (CursorDisplayOn && ((CRTRegister[0xA] & 0x1f) <= (CRTRegister[0xB] & 0x1F)))
  {
    SDL_Rect crect;
    crect.x = (CursorLocation % 40) * 16 * Scaling;
    crect.y = DrawTop + ((CursorLocation / 40) * 16 + (CRTRegister[0xA] & 0x1f) * 2) * Scaling;
    crect.w = 16 * Scaling;
    crect.h = DrawTop + ((CursorLocation / 40) * 16 + (CRTRegister[0xB] & 0x1F) * 2 + 2) * Scaling - crect.y;
    SDL_FillRect(SDL_GetWindowSurface(hwnd), &crect, 0x00FFFFFF);
  }

  ScreenFullRedraw = false;
}

static void VGA8_DrawCO40(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xb8000 + PageOffset;
  unsigned char *cm = TextState;
  unsigned char *gd;
  unsigned char *bm;
  unsigned char *ci;
  int attr;
  int glyph;
  int gx, gy;
  int fgColour;
  int bgColour;
  unsigned char Mask;

  for (int y = 0 ; y < 25 ; y++)
  {

    for (int x = 0 ; x < 40 ; x++)
    {
      bm = ((unsigned char *) GFX640x480Surface->pixels) + ((y * 640 * 16 + x * 8) * BytesPerPixel);

      glyph = *(vm++);
      attr = *(vm++);

      if (ScreenFullRedraw || (glyph != cm[0]) || (attr != cm[1]))
      {
        fgColour = attr & 0x0f;
        bgColour = (attr >> 4) & 0x0f;

        gd = VGAGlyphs + (glyph << 4);

        for (gy = 0 ; gy < 16 ; gy++)
        {
          Mask = 0x80;
          for (gx = 0 ; gx < 8 ; gx++)
          {
            if ((*gd) & Mask)
            {
              ci = CGAPaletteB + fgColour*3;
            }
            else
            {
              ci = CGAPaletteB + bgColour*3;
            }

            *bm++ = *ci++;
            *bm++ = *ci++;
            *bm++ = *ci++;
            if (BytesPerPixel > 3) bm++;

            Mask >>= 1;
          }
          bm += 632 * BytesPerPixel;
          gd++;
        }

        cm[0] = glyph;
        cm[1] = attr;
      }
      cm+=2;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 320;
  SRect.h = 400;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX640x480Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  UpdateCursorstate();

  if (CursorDisplayOn && ((CRTRegister[0xA] & 0x1f) <= (CRTRegister[0xB] & 0x1F)))
  {
    SDL_Rect crect;
    crect.x = (CursorLocation % 40) * 16 * Scaling;
    crect.y = DrawTop + ((CursorLocation / 40) * 16 + (CRTRegister[0xA] & 0x1f) * 2) * Scaling;
    crect.w = 16 * Scaling;
    crect.h = DrawTop + ((CursorLocation / 40) * 16 + (CRTRegister[0xB] & 0x1F) * 2 + 2) * Scaling - crect.y;
    SDL_FillRect(SDL_GetWindowSurface(hwnd), &crect, 0x00FFFFFF);
  }

  ScreenFullRedraw = false;
}

static void CGA_DrawCO80(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xb8000 + PageOffset;
  unsigned char *cm = TextState;
  unsigned char *gd;
  unsigned char *bm;
  unsigned char *ci;
  int attr;
  int glyph;
  int gx, gy;
  int fgColour;
  int bgColour;
  unsigned char Mask;

  for (int y = 0 ; y < 25 ; y++)
  {

    for (int x = 0 ; x < 80 ; x++)
    {
      bm = ((unsigned char *) GFX640Surface->pixels) + ((y * 640 * 8 + x * 8) * BytesPerPixel);

      glyph = *(vm++);
      attr = *(vm++);

      if (ScreenFullRedraw || (glyph != cm[0]) || (attr != cm[1]))
      {
        fgColour = attr & 0x0f;
        bgColour = (attr >> 4) & 0x0f;

        gd = CGAGlyphs + (glyph << 3);

        for (gy = 0 ; gy < 8 ; gy++)
        {
          Mask = 0x80;
          for (gx = 0 ; gx < 8 ; gx++)
          {
            if ((*gd) & Mask)
            {
              ci = CGAPaletteB + fgColour*3;
            }
            else
            {
              ci = CGAPaletteB + bgColour*3;
            }

            *bm++ = *ci++;
            *bm++ = *ci++;
            *bm++ = *ci++;
            if (BytesPerPixel > 3) bm++;

            Mask >>= 1;
          }
          bm += 632 * BytesPerPixel;
          gd++;
        }

        cm[0] = glyph;
        cm[1] = attr;
      }
      cm+=2;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 640;
  SRect.h = 200;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX640Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  UpdateCursorstate();

  if (CursorDisplayOn && ((CRTRegister[0xA] & 0x1f) <= (CRTRegister[0xB] & 0x1F)))
  {
    SDL_Rect crect;
    crect.x = (CursorLocation % 80) * 8 * Scaling;
    crect.y = DrawTop + ((CursorLocation / 80) * 16 + (CRTRegister[0xA] & 0x1f) * 2) * Scaling;
    crect.w = 8 * Scaling;
    crect.h = DrawTop + ((CursorLocation / 80) * 16 + (CRTRegister[0xB] & 0x1F) * 2 + 2) * Scaling - crect.y;
    SDL_FillRect(SDL_GetWindowSurface(hwnd), &crect, 0x00FFFFFF);
  }

  ScreenFullRedraw = false;
}

static void VGA8_DrawCO80(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xb8000 + PageOffset;
  unsigned char *cm = TextState;
  unsigned char *gd;
  unsigned char *bm;
  unsigned char *ci;
  int attr;
  int glyph;
  int gx, gy;
  int fgColour;
  int bgColour;
  unsigned char Mask;

  for (int y = 0 ; y < 25 ; y++)
  {

    for (int x = 0 ; x < 80 ; x++)
    {
      bm = ((unsigned char *) GFX640x480Surface->pixels) + ((y * 640 * 16 + x * 8) * BytesPerPixel);

      glyph = *(vm++);
      attr = *(vm++);

      if (ScreenFullRedraw || (glyph != cm[0]) || (attr != cm[1]))
      {
        fgColour = attr & 0x0f;
        bgColour = (attr >> 4) & 0x0f;

        gd = VGAGlyphs + (glyph << 4);

        for (gy = 0 ; gy < 16 ; gy++)
        {
          Mask = 0x80;
          for (gx = 0 ; gx < 8 ; gx++)
          {
            if ((*gd) & Mask)
            {
              ci = CGAPaletteB + fgColour*3;
            }
            else
            {
              ci = CGAPaletteB + bgColour*3;
            }

            *bm++ = *ci++;
            *bm++ = *ci++;
            *bm++ = *ci++;
            if (BytesPerPixel > 3) bm++;

            Mask >>= 1;
          }
          bm += 632 * BytesPerPixel;
          gd++;
        }

        cm[0] = glyph;
        cm[1] = attr;
      }
      cm+=2;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 640;
  SRect.h = 400;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX640x480Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  UpdateCursorstate();

  if (CursorDisplayOn && ((CRTRegister[0xA] & 0x1f) <= (CRTRegister[0xB] & 0x1F)))
  {
    SDL_Rect crect;
    crect.x = (CursorLocation % 80) * 8 * Scaling;
    crect.y = DrawTop + ((CursorLocation / 80) * 16 + (CRTRegister[0xA] & 0x1f) * 2) * Scaling;
    crect.w = 8 * Scaling;
    crect.h = DrawTop + ((CursorLocation / 80) * 16 + (CRTRegister[0xB] & 0x1F) * 2 + 2) * Scaling - crect.y;
    SDL_FillRect(SDL_GetWindowSurface(hwnd), &crect, 0x00FFFFFF);
  }

  ScreenFullRedraw = false;
}

static void CGA_DrawCO320(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xb8000 + PageOffset * 2;
  unsigned char *bm;
  unsigned char *ci;
  int c0, c1, c2, c3;

  for (int y = 0 ; y < 200 ; y += 2)
  {
    bm = ((unsigned char *) GFX320Surface->pixels) + (y * 320 * BytesPerPixel);

    for (int x = 0 ; x < 320 ; x += 4)
    {
      c0 = ((*vm) >> 6) & 0x03;
      c1 = ((*vm) >> 4) & 0x03;
      c2 = ((*vm) >> 2) & 0x03;
      c3 = (*vm) & 0x03;

      if (c0 != 0) c0 = 27 + c0 * 6;
      if (c1 != 0) c1 = 27 + c1 * 6;
      if (c2 != 0) c2 = 27 + c2 * 6;
      if (c3 != 0) c3 = 27 + c3 * 6;

      ci = MCGAPalette + c0;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = MCGAPalette + c1;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = MCGAPalette + c2;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = MCGAPalette + c3;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      vm++;
      if (vm > (mem + 0xba000)) vm -= 8192;
    }
  }

  vm = mem + 0xba000 + PageOffset * 2;
  for (int y = 1 ; y < 200 ; y += 2)
  {
    bm = ((unsigned char *) GFX320Surface->pixels) + (y * 320 * BytesPerPixel);

    for (int x = 0 ; x < 320 ; x += 4)
    {
      c0 = ((*vm) >> 6) & 0x03;
      c1 = ((*vm) >> 4) & 0x03;
      c2 = ((*vm) >> 2) & 0x03;
      c3 = (*vm) & 0x03;

      if (c0 != 0) c0 = 27 + c0 * 6;
      if (c1 != 0) c1 = 27 + c1 * 6;
      if (c2 != 0) c2 = 27 + c2 * 6;
      if (c3 != 0) c3 = 27 + c3 * 6;

      ci = MCGAPalette + c0;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = MCGAPalette + c1;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = MCGAPalette + c2;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = MCGAPalette + c3;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      vm++;
      if (vm > (mem + 0xbc000)) vm -= 8192;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 320;
  SRect.h = 200;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX320Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  ScreenFullRedraw = false;
}

static void CGA_Draw640(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xb8000;
  unsigned char *bm;
  unsigned char *ci;
  int c0, c1, c2, c3, c4, c5, c6, c7;

  for (int y = 0 ; y < 200 ; y += 2)
  {
    bm = ((unsigned char *) GFX640Surface->pixels) + (y * 640 * BytesPerPixel);

    for (int x = 0 ; x < 640 ; x += 8)
    {
      c0 = ((*vm) >> 7) & 0x01;
      c1 = ((*vm) >> 6) & 0x01;
      c2 = ((*vm) >> 5) & 0x01;
      c3 = ((*vm) >> 4) & 0x01;
      c4 = ((*vm) >> 3) & 0x01;
      c5 = ((*vm) >> 2) & 0x01;
      c6 = ((*vm) >> 1) & 0x01;
      c7 = (*vm) & 0x01;

      ci = c0 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c1 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c2 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c3 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c4 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c5 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c6 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c7 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      vm++;
    }
  }

  vm = mem + 0xba000;
  for (int y = 1 ; y < 200 ; y += 2)
  {
    bm = ((unsigned char *) GFX640Surface->pixels) + (y * 640 * BytesPerPixel);

    for (int x = 0 ; x < 320 ; x += 4)
    {
      c0 = ((*vm) >> 7) & 0x01;
      c1 = ((*vm) >> 6) & 0x01;
      c2 = ((*vm) >> 5) & 0x01;
      c3 = ((*vm) >> 4) & 0x01;
      c4 = ((*vm) >> 3) & 0x01;
      c5 = ((*vm) >> 2) & 0x01;
      c6 = ((*vm) >> 1) & 0x01;
      c7 = (*vm) & 0x01;

      ci = c0 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c1 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c2 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c3 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c4 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c5 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c6 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      ci = c7 ? CGAPaletteB + CGA320Palette[0]*3 : CGAPaletteB;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      vm++;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 640;
  SRect.h = 200;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX640Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  ScreenFullRedraw = false;
}

static void MCGA_DrawMode11(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xa0000;
  unsigned char *bm;
  unsigned char ci;
  register unsigned char c;

  for (int y = 0 ; y < 480 ; y++)
  {
    bm = ((unsigned char *) GFX640x480Surface->pixels) + (y * 640 * BytesPerPixel);

    for (int x = 0 ; x < 640 ; x += 8)
    {
      c = *vm;

      ci = (c & 0x80) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x40) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x20) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x10) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x08) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x04) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x02) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      ci = (c & 0x01) ? 0xff : 0x00;
      *bm++ = ci;
      *bm++ = ci;
      *bm++ = ci;
      if (BytesPerPixel > 3) bm++;

      vm++;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 640;
  SRect.h = 480;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 480 * Scaling;

  SDL_BlitScaled(
    GFX640x480Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  ScreenFullRedraw = false;
}

static void MCGA_DrawMode13(SDL_Window *hwnd, unsigned char *mem)
{
  unsigned char *vm = mem + 0xa0000;
  unsigned char *bm;
  unsigned char *ci;
  int c0;

  for (int y = 0 ; y < 200 ; y++)
  {
    bm = ((unsigned char *) GFX320Surface->pixels) + (y * 320 * BytesPerPixel);

    for (int x = 0 ; x < 320 ; x++)
    {
      c0 = *vm;

      ci = MCGAPalette + c0*3;
      *bm++ = *ci++;
      *bm++ = *ci++;
      *bm++ = *ci++;
      if (BytesPerPixel > 3) bm++;

      vm++;
    }
  }

  SDL_Rect SRect;
  SRect.x = 0;
  SRect.y = 0;
  SRect.w = 320;
  SRect.h = 200;

  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = DrawTop;
  DRect.w = 640 * Scaling;
  DRect.h = 400 * Scaling;

  SDL_BlitScaled(
    GFX320Surface,
    &SRect,
    SDL_GetWindowSurface(hwnd),
    &DRect);

  ScreenFullRedraw = false;
}

void DetermineGfxMode(void)
{
  // Sequencer Register 4 always has Odd/Even set for
  // when in CGA emulation.
  if ((SQRegisters[4] & 0x04) != 0)
  {
    // CGA Emulation mode

    // CGA video mode
    if ((CGAModeControlRegister & 0x02) == 0)
    {
      // text mode

      if ((CGAModeControlRegister & 0x01) == 0)
      {
        // 40 column
        CurrentScreenMode = SM_BW40;
      }
      else
      {
        // 80 column
        CurrentScreenMode = SM_BW80;
      }

      if ((CGAModeControlRegister & 0x04) == 0)
      {
        // colour
        CurrentScreenMode = (ScreenMode_t) (CurrentScreenMode + 1);
      }

      // Restore the default palette
      for (int i = 0 ; i < 48 ; i++)
      {
        MCGAPalette[i] = CGAPaletteB[i];
      }

      CGA320Palette[0] = CGAColourControlRegister & 0x0f;

      ScreenFullRedraw = true;
    }
    else
    {
      // graphics mode
      if ((CGAModeControlRegister & 0x10) != 0)
      {
        CurrentScreenMode = SM_640x200;

        // Restore the default palette
        for (int i = 0 ; i < 48 ; i++)
        {
          MCGAPalette[i] = CGAPaletteB[i];
        }
      }
      else
      {
        CurrentScreenMode = SM_CO320;

        // Set colour palette for CGA modes
        if ((CGAModeControlRegister & 0x04) != 0)
        {
          CGA320Palette = CGA320Palette5;
        }
        else
        {
          if ((CGAColourControlRegister & 0x20) == 0)
          {
            if ((CGAColourControlRegister & 0x10) == 0)
            {
              CGA320Palette = CGA320Palette1;
            }
            else
            {
              CGA320Palette = CGA320Palette3;
            }
          }
          else
          {
            if ((CGAColourControlRegister & 0x10) == 0)
            {
              CGA320Palette = CGA320Palette2;
            }
            else
            {
              CGA320Palette = CGA320Palette4;
            }
          }
        }

        CGA320Palette[0] = CGAColourControlRegister & 0x0f;

        // Load the palette entries
        MCGAPalette[0] = CGAPaletteB[CGA320Palette[0]*3];
        MCGAPalette[1] = CGAPaletteB[CGA320Palette[0]*3+1];
        MCGAPalette[2] = CGAPaletteB[CGA320Palette[0]*3+2];

        MCGAPalette[33] = CGAPaletteB[CGA320Palette[1]*3];
        MCGAPalette[34] = CGAPaletteB[CGA320Palette[1]*3+1];
        MCGAPalette[35] = CGAPaletteB[CGA320Palette[1]*3+2];

        MCGAPalette[39] = CGAPaletteB[CGA320Palette[2]*3];
        MCGAPalette[40] = CGAPaletteB[CGA320Palette[2]*3+1];
        MCGAPalette[41] = CGAPaletteB[CGA320Palette[2]*3+2];

        MCGAPalette[45] = CGAPaletteB[CGA320Palette[3]*3];
        MCGAPalette[46] = CGAPaletteB[CGA320Palette[3]*3+1];
        MCGAPalette[47] = CGAPaletteB[CGA320Palette[3]*3+2];
      }
    }
  }
  else
  {
    // Very basic check for mode 11h/13h.
    // This could probably be improved, but the Graphics Mode
    // register will work well enough
    if ((GCRegisters[5] & 0x40) != 0)
    {
      CurrentScreenMode = SM_MODE13;
    }
    else
    {
      CurrentScreenMode = SM_MODE11;
    }
  }


}

// =============================================================================
// Exported Functions
//

void CGA_Initialise(SDL_Window *hwnd, int Top)
{
  SDL_Surface *WinSurf = SDL_GetWindowSurface(hwnd);
  DrawTop = Top;

  BPP = WinSurf->format->BitsPerPixel;
  BytesPerPixel = BPP / 8;

  // Create the 320x200 bitmap work area
  GFX320Surface = SDL_CreateRGBSurface(
    0, 320, 200,
    WinSurf->format->BitsPerPixel,
    WinSurf->format->Rmask,
    WinSurf->format->Gmask,
    WinSurf->format->Bmask,
    WinSurf->format->Amask);
  if (GFX320Surface == NULL)
  {
    printf("Create surface 320x200 failed\n");
  }

  // Create the 640x200 bitmap work area
  GFX640Surface = SDL_CreateRGBSurface(
    0, 640, 200,
    WinSurf->format->BitsPerPixel,
    WinSurf->format->Rmask,
    WinSurf->format->Gmask,
    WinSurf->format->Bmask,
    WinSurf->format->Amask);
  if (GFX640Surface == NULL)
  {
    printf("Create surface 640x200 failed\n");
  }

  // Create the 640x480 bitmap work area
  GFX640x480Surface = SDL_CreateRGBSurface(
    0, 640, 480,
    WinSurf->format->BitsPerPixel,
    WinSurf->format->Rmask,
    WinSurf->format->Gmask,
    WinSurf->format->Bmask,
    WinSurf->format->Amask);
  if (GFX640x480Surface == NULL)
  {
    printf("Create surface 640x480 failed\n");
  }

  for (int i = 0 ; i < 80*25*2 ; i++)
  {
    TextState[i] = 0;
  }

  CursorBlinkTime = SDL_GetTicks() + 500;

  CGA_Reset();
}

void CGA_Reset(void)
{
  // Copy the default palette into the first 16 entries
  // of the MCGA palette
  for (int i = 0 ; i < 48 ; i++)
  {
    MCGAPalette[i] = CGAPaletteB[i];
  }

  CGAModeControlRegister = 0;
  CGAColourControlRegister = 0;

  // Reset registers
  ACIndexState = true;
  ACIndex = 0;
  for (int i = 0 ; i < AC_REG_COUNT ; i++)
  {
    ACRegisters[i] = DefACRegisters[i];
  }

  MiscOutputReg = 0x67;

  ColourReadIndex = 0;
  ColourReadComponent = 0;
  ColourWriteIndex = 0;
  ColourWriteComponent = 0;

  SQIndex = 0;
  for (int i = 0 ; i < SQ_REG_COUNT ; i++)
  {
    SQRegisters[i] = DefSQRegisters[i];
  }

  GCIndex = 0;
  for (int i = 0 ; i < GC_REG_COUNT ; i++)
  {
    GCRegisters[i] = DefGCRegisters[i];
  }

  PageOffset = 0;
  CursorLocation = 0;
  CursorDisplayOn = false;
  CursorBlinkTime = 0;
  CGAStatus = 0;
  CGARetraceEndTime = 0;

  CurrentScreenMode = SM_CO80;

  ScreenFullRedraw = true;
}

void CGA_Cleanup(void)
{
  if (GFX320Surface != NULL)
  {
    SDL_FreeSurface(GFX320Surface);
    GFX320Surface = NULL;
  }

  if (GFX640Surface != NULL)
  {
    SDL_FreeSurface(GFX640Surface);
    GFX640Surface = NULL;
  }
}

unsigned int CGA_VMemRead(unsigned char *mem, int i_w, int addr)
{
  if (i_w)
  {
    // Reading a word should set the latch register to the MSB of the word read
    LatchRegisters[0] = mem[addr+1];
    return mem[addr] | (mem[addr+1] << 8);
  }
  else
  {
    LatchRegisters[0] = mem[addr];
    return mem[addr];
  }
}

static inline void CGA_WriteByte(unsigned char *mem, int addr, unsigned char val)
{
  unsigned char setreset_val;
  unsigned char logic_val;
  unsigned char mask_val;

  if (WriteMode == 1)
  {
    // Write mode 1 sets the memory to the content of the latch register
    mem[addr] = LatchRegisters[0];
    return;
  }
  else if (WriteMode == 3)
  {
    // MCGA write mode 3 does not appear to be the same as VGA.
    // It seems to be a pass through mode.
    mem[addr] = val;
    return;
  }

  // Write mode is 0, 2

  // Calculate the set/reset value based on mode
  if (WriteMode == 0)
  {
    if (GCRegisters[1] & 0x01)
    {
      setreset_val = (GCRegisters[0] & 0x01) ? 0xff : 0x00;
    }
    else
    {
      if (RotateCount != 0)
      {
        setreset_val = (unsigned char) ((val << RotateCount) | (val >> (8-RotateCount)));
      }
      else
      {
        setreset_val = val;
      }
    }
  }
  else
  {
    setreset_val = (val & 0x01) ? 0xff : 0x00;
  }

  // Write modes 0 and 2 are the same at this point onwards

  switch (LogicOp)
  {
    case 0:
      logic_val = setreset_val;
      break;
    case 1:
      logic_val = setreset_val & LatchRegisters[0];
      break;
    case 2:
      logic_val = setreset_val | LatchRegisters[0];
      break;
    case 3:
      logic_val = setreset_val ^ LatchRegisters[0];
      break;
    default:
      logic_val = 0;
      break;
  }

  mask_val = GCRegisters[8];
  mem[addr] = (logic_val & mask_val) | (LatchRegisters[0] & (~mask_val));
}

unsigned int CGA_VMemWrite(unsigned char *mem, int i_w, int addr, unsigned int val)
{
  unsigned char tmp_val;

  // Process least significant byte
  tmp_val = val & 0x00ff;
  CGA_WriteByte(mem, addr, tmp_val);

  // If 16 bit access the write most significant byte
  if (i_w)
  {
    tmp_val = (val >> 8) & 0x00ff;
    CGA_WriteByte(mem, addr+1, tmp_val);
  }

  return 0;
}


bool CGA_WritePort(int Address, unsigned char Val)
{
  bool Handled = false;

  switch (Address)
  {
    case 0x3B4:
    case 0x3D4:
      Handled = true;
      CRTIndexRegister = Val;
      break;

    case 0x3B5:
    case 0x3D5:
      Handled = true;
      if (CRTIndexRegister < 16) CRTRegister[CRTIndexRegister] = Val;
      switch (CRTIndexRegister)
      {
        case 0x0A:
        case 0x0B:
          // Nothing to do for cursor shape.
          // Cursor shape processing is perform in the screen drawing
          // functions from the register values.
          break;

        case 0x0C:
        case 0x0D:
          PageOffset = (CRTRegister[0x0C] << 8) + CRTRegister[0x0D];
          break;

        case 0x0E:
        case 0x0F:
          CursorLocation = (CRTRegister[0x0E] << 8) + CRTRegister[0x0F];
          break;

        default:
          //printf("Write to CRTC index = 0x%02x value = 0x%02x\n", CRTIndexRegister, Val);
          break;
      }

    case 0x03ba:
      Handled = true;
      break;

    case 0x03c0:
      Handled = true;
      if (ACIndexState)
      {
        ACIndex = Val;
      }
      else
      {
        if (ACIndex < AC_REG_COUNT)
        {
          ACRegisters[ACIndex] = Val;
        }
      }
      ACIndexState = !ACIndexState;
      break;

    case 0x03c2:
      Handled = true;
      MiscOutputReg = Val;
      break;

    case 0x03c4:
      Handled = true;
      SQIndex = Val;
      break;

    case 0x03c5:
      Handled = true;
      if (SQIndex < SQ_REG_COUNT)
      {
        SQRegisters[SQIndex] = Val;
      }
      break;

    case 0x03c7:
      Handled = true;
      ColourReadIndex = Val;
      ColourReadComponent = 0;
      break;

    case 0x03c8:
      Handled = true;
      ColourWriteIndex = Val;
      ColourWriteComponent = 0;
      break;

    case 0x03c9:
      Handled = true;
      MCGAPalette[ColourWriteIndex * 3 + 2-ColourWriteComponent] = (Val << 2);
      ColourWriteComponent++;
      if (ColourWriteComponent == 3)
      {
        ColourWriteComponent = 0;
        ColourWriteIndex++;
      }
      break;

    case 0x03ce:
      Handled = true;
      GCIndex = Val;
      break;

    case 0x03cf:
      Handled = true;
      if (GCIndex < GC_REG_COUNT)
      {
        GCRegisters[GCIndex] = Val;

        switch (GCIndex)
        {
          case 3:
            RotateCount = (Val & 0x07);
            LogicOp = (Val >> 3) & 0x03;
            break;

          case 4:
            break;

          case 5:
            WriteMode = Val & 0x03;
            ReadMode = (Val >> 3) & 0x01;
            HostOE = (Val >> 4) & 0x01;
            break;

          case 8:
            // Mask register
            break;

          default:
            break;
        }

        DetermineGfxMode();
      }
      break;

    case 0x03d8:
      Handled = true;
      CGAModeControlRegister = Val;
      DetermineGfxMode();
      break;

    case 0x03d9:
      Handled = true;
      CGAColourControlRegister = Val;
      DetermineGfxMode();
      break;

    default:
      break;
  }

  return Handled;
}

bool CGA_ReadPort(int Address, unsigned char &Val)
{
  bool Handled = false;
  unsigned int CurrentTime;

  // Handle specific processing for ports that do something different.
  switch (Address)
  {
    case 0x3BA:
      Handled = true;
      break;

    case 0x03c0:
      Handled = true;
      Val = ACIndex;
      break;

    case 0x03c1:
      Handled = true;
      if (ACIndex < AC_REG_COUNT)
      {
        Val = ACRegisters[ACIndex];
      }
      else
      {
        Val = 0;
      }
      break;

    case 0x03c4:
      Handled = true;
      Val = SQIndex;
      break;

    case 0x03c5:
      Handled = true;
      if (SQIndex < SQ_REG_COUNT)
      {
        Val = SQRegisters[SQIndex];
      }
      else
      {
        Val = 0;
      }
      break;

    case 0x03c8:
      Handled = true;
      Val = ColourWriteIndex;
      break;

    case 0x03c9:
      Handled = true;
      Val = MCGAPalette[ColourReadIndex * 3 + 2-ColourReadComponent] >> 2;
      ColourReadComponent++;
      if (ColourReadComponent == 3)
      {
        ColourReadComponent = 0;
        ColourReadIndex++;
      }
      break;

    case 0x03cc:
      Handled = true;
      Val = MiscOutputReg;
      break;

    case 0x03ce:
      Handled = true;
      Val = GCIndex;
      break;

    case 0x03cf:
      Handled = true;
      if (GCIndex < GC_REG_COUNT)
      {
        Val = GCRegisters[GCIndex];
      }
      else
      {
        Val = 0;
      }
      break;

    case 0x3D8:
      Handled = true;
      Val = CGAModeControlRegister;
      break;

    case 0x3D9:
      Handled = true;
      Val = CGAColourControlRegister;
      break;

    case 0x3DA:
      Handled = true;
      // handle vblank at some approximation of accurate.
      CurrentTime = SDL_GetTicks();
      if (CurrentTime > CGARetraceEndTime)
      {
        // clear retrace
        CGAStatus &= 0xf7;
      }
      Val = CGAStatus;

      // VMem access goes high/low every scan line.
      // This is pretty often, so just alternate it every access.
      CGAStatus ^= 0x01;

      // Reading this register sets Attribute Controller to
      // set the index on the next write to 0x03c0
      ACIndexState = true;
      break;

    default:
      break;
  }

  return Handled;
}

void CGA_VBlankStart(void)
{
  unsigned int CurrentTime = SDL_GetTicks();
  CGAStatus |= 0x08;
  CGARetraceEndTime = CurrentTime + 2;
}

void CGA_SetTextDisplay(TextDisplay_t Mode)
{
  TextDisplay = Mode;
  ScreenFullRedraw = true;
}

void CGA_SetScale(int Scale)
{
  Scaling = Scale;
}

void CGA_GetDisplaySize(int &w, int &h)
{
  if (CurrentScreenMode == SM_MODE11)
  {
    w = 640 * Scaling;
    h = 480 * Scaling;
  }
  else
  {
    w = 640 * Scaling;
    h = 400 * Scaling;
  }
}

void CGA_DrawScreen(SDL_Window *hwnd, unsigned char *mem)
{
  //unsigned int tickStart = SDL_GetTicks();

  if (CurrentScreenMode == SM_BW40)
  {
    if (TextDisplay == TD_CGA)
    {
      CGA_DrawCO40(hwnd, mem);
    }
    else if (TextDisplay == TD_VGA_8x16)
    {
      VGA8_DrawCO40(hwnd, mem);
    }
  }
  else if (CurrentScreenMode == SM_CO40)
  {
    if (TextDisplay == TD_CGA)
    {
      CGA_DrawCO40(hwnd, mem);
    }
    else if (TextDisplay == TD_VGA_8x16)
    {
      VGA8_DrawCO40(hwnd, mem);
    }
  }
  else if (CurrentScreenMode == SM_BW80)
  {
    if (TextDisplay == TD_CGA)
    {
      CGA_DrawCO80(hwnd, mem);
    }
    else if (TextDisplay == TD_VGA_8x16)
    {
      VGA8_DrawCO80(hwnd, mem);
    }
  }
  else if (CurrentScreenMode == SM_CO80)
  {
    if (TextDisplay == TD_CGA)
    {
      CGA_DrawCO80(hwnd, mem);
    }
    else if (TextDisplay == TD_VGA_8x16)
    {
      VGA8_DrawCO80(hwnd, mem);
    }
  }
  else if (CurrentScreenMode == SM_CO320)
  {
    CGA_DrawCO320(hwnd, mem);
  }
  else if (CurrentScreenMode == SM_640x200)
  {
    CGA_Draw640(hwnd, mem);
  }
  else if (CurrentScreenMode == SM_MODE11)
  {
    MCGA_DrawMode11(hwnd, mem);
  }
  else if (CurrentScreenMode == SM_MODE13)
  {
    MCGA_DrawMode13(hwnd, mem);
  }

  SDL_UpdateWindowSurface(hwnd);

  //unsigned int tickDelta = SDL_GetTicks() - tickStart;
  //printf("Screen draw took %d ticks\n", tickDelta);

}
