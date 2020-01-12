/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_edit.cpp
 *
 * DESCRIPTION:
 * This module defines the SDLEdit Control class.
 * This class is derived from the SDLControl class and defines an edit box
 * control for use in a SDLControlList.
 *
 * =============================================================================
 * COPYRIGHT:
 *
 * Copyright (c) 2005, Julian Olds
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   . Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *   . Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * The name of the author may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * =============================================================================
 * EXPORTED VARIABLES
 *
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 *
 * =============================================================================
 */

#include <string.h>
#include <ctype.h>

#include "sdl_edit.h"
#include "sdl_simple_text.h"


static Uint32 myTimerCallback(Uint32 interval, void *param)
{
  SDL_Event event;
  SDL_UserEvent userevent;

  /* In this example, our callback pushes a function
  into the queue, and causes our callback to be called again at the
  same interval: */

  userevent.type = SDL_USEREVENT;
  userevent.code = 0;
  userevent.data1 = 0;
  userevent.data2 = param;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return(interval);
}

// =============================================================================
// Class: SDLEdit_t
//

SDLEdit_t::SDLEdit_t(int ControlId, struct SDLControlData_t &Data, int BufferLen)
  : SDLControl_t(ControlId, Data)
{
  HasFocus = false;
  if (BufferLen == 0) BufferLen = 64;

  TextBufferLen = BufferLen;
  TextLen = 0;
  TextBuffer = new char[TextBufferLen];

  IsCursorIBEAM = false;
  Cursor_System = SDL_GetCursor();
  Cursor_IBEAM = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
}

SDLEdit_t::~SDLEdit_t()
{
  SDL_FreeCursor(Cursor_IBEAM);
  delete[] TextBuffer;
  if (HasFocus)
  {
    SDL_RemoveTimer(BlinkTimerId);
  }
}

void SDLEdit_t::Draw(SDL_Surface *Surf)
{
  int text_w, text_h;
  SDLTEXT_GetSize(TextBuffer, text_w, text_h, strlen(TextBuffer));

  SDL_FillRect(Surf, &ControlData.Rect, ControlData.ImageData[State].FaceColour);

  Draw3DRect(Surf,
             ControlData.Rect,
             ControlData.ImageData[State].LoColour,
             ControlData.ImageData[State].HiColour);

  int cx = ControlData.Rect.x + (ControlData.Rect.w / 2);
  int cy = ControlData.Rect.y + (ControlData.Rect.h / 2);
  int tx = cx - text_w / 2;
  int ty = cy - text_h / 2;

  SDLTEXT_DrawString(Surf, tx, ty, ControlData.ImageData[State].TextColour, TextBuffer);

  if (HasFocus)
  {
    int Ticks = SDL_GetTicks();

    if (Ticks > CursorTicks)
    {
      CursorBlinkOn = !CursorBlinkOn;
      CursorTicks += 500;
    }

    if (CursorBlinkOn)
    {
      SDLTEXT_GetSize(TextBuffer, cx, cy, CursorPos);
      SDL_Rect CRect;
      CRect.x = tx + cx;
      CRect.y = ty;
      CRect.w = 2;
      CRect.h = cy;

      SDL_FillRect(Surf, &CRect, 0x00000000);
    }
  }
}

bool SDLEdit_t::HandleEvent(SDL_Event &Event)
{
  bool ReportEvent = false;
  int x, y;

  if (!IsActive)
  {
    return false;
  }

  switch (Event.type)
  {
    case SDL_MOUSEBUTTONDOWN:
    {
      switch (Event.button.button)
      {
        case SDL_BUTTON_LEFT:
          x = Event.button.x;
          y = Event.button.y;

          if (IsHit(x, y))
          {
            if (!HasFocus)
            {
              CursorPos = strlen(TextBuffer);
              CursorBlinkOn = true;
              CursorTicks = SDL_GetTicks() + 500;
              BlinkTimerId = SDL_AddTimer(100, myTimerCallback, NULL);
            }
            HasFocus = true;
          }
          else
          {
            if (HasFocus)
            {
              ReportEvent = true;
            }
            HasFocus = false;
            SDL_RemoveTimer(BlinkTimerId);
          }

        default:
          break;
      }
      break;
    }

    case SDL_MOUSEMOTION:
    {
      x = Event.motion.x;
      y = Event.motion.y;

      if (IsHit(x, y))
      {
        State = SDLCONTROL_STATE_ACTIVE_HILIGHT;
        if (!IsCursorIBEAM)
        {
          SDL_SetCursor(Cursor_IBEAM);
          IsCursorIBEAM = true;
        }
      }
      else
      {
        State = SDLCONTROL_STATE_ACTIVE_NORMAL;
        if (IsCursorIBEAM)
        {
          SDL_SetCursor(Cursor_System);
          IsCursorIBEAM = false;
        }
      }
      break;
    }

    case SDL_KEYDOWN:
    {
      if (HasFocus)
      {
        switch (Event.key.keysym.sym)
        {
          case SDLK_BACKSPACE:
          {
            if (CursorPos > 0)
            {
              int dst = CursorPos-1;
              int src = CursorPos;
              do
              {
                TextBuffer[dst++] = TextBuffer[src++];
              } while (TextBuffer[dst-1] != 0);
              CursorPos--;
            }
            break;
          }

          case SDLK_LEFT:
          {
            if (CursorPos > 0) CursorPos--;
            break;
          }

          case SDLK_RIGHT:
          {
            if (CursorPos < strlen(TextBuffer)) CursorPos++;
            break;
          }

          case SDLK_RETURN:
          {
            ReportEvent = true;
            HasFocus = false;
            break;
          }
        }
      }

      break;
    }

    case SDL_TEXTINPUT:
    {
      if (HasFocus)
      {
        int i = 0;
        char ch;
        while (Event.text.text[i] != 0)
        {
          ch = Event.text.text[i];

          if (isprint(ch))
          {
            if (strlen(TextBuffer) < (TextBufferLen-1))
            {
              int src = strlen(TextBuffer);
              int dst = src+1;
              while (src >= CursorPos)
              {
                TextBuffer[dst--] = TextBuffer[src--];
              }
              TextBuffer[CursorPos] = ch;
              CursorPos++;
            }
          }

          i++;
        }
      }
      break;
    }

    case SDL_USEREVENT:
    {
      break;
    }

    default:
      break;

  }

  return ReportEvent;
}

void SDLEdit_t::SetText(const char *Text)
{
  strncpy(TextBuffer, Text, TextBufferLen);
  TextBuffer[TextBufferLen-1] = 0;
  TextLen = strlen(TextBuffer);
}

void SDLEdit_t::GetText(char *Text, int *Length)
{
  strncpy(Text, TextBuffer, *Length);
  Text[(*Length)-1] = 0;

  *Length = strlen(Text);
}

