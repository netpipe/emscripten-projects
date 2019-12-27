/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_button.cpp
 *
 * DESCRIPTION:
 * This module defines the SDLButton Control class.
 * This class is derived from the SDLControl class and defines a push button
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

#include "sdl_button.h"
#include "sdl_simple_text.h"

SDLButton_t::SDLButton_t(int ControlId, struct SDLControlData_t &Data)
  : SDLControl_t(ControlId, Data)
{
  ButtonPressed = false;
}

SDLButton_t::~SDLButton_t()
{
}

void SDLButton_t::Draw(SDL_Surface *Surf)
{
  int text_w, text_h;
  SDLTEXT_GetSize(ControlData.Text, text_w, text_h, strlen(ControlData.Text));

  SDL_FillRect(Surf, &ControlData.Rect, ControlData.ImageData[State].FaceColour);

  Draw3DRect(Surf,
             ControlData.Rect,
             ControlData.ImageData[State].HiColour,
             ControlData.ImageData[State].LoColour);

  int cx = ControlData.Rect.x + (ControlData.Rect.w / 2);
  int cy = ControlData.Rect.y + (ControlData.Rect.h / 2);
  int tx = cx - text_w / 2;
  int ty = cy - text_h / 2;

  SDLTEXT_DrawString(Surf, tx, ty, ControlData.ImageData[State].TextColour, ControlData.Text);
}

bool SDLButton_t::HandleEvent(SDL_Event &Event)
{
  bool ButtonEvent = false;
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
            State = SDLCONTROL_STATE_ACTIVE_PRESSED;
            ButtonPressed = true;
          }

        default:
          break;
      }
      break;
    }

   case SDL_MOUSEBUTTONUP:
    {
      switch (Event.button.button)
      {
        case SDL_BUTTON_LEFT:
          x = Event.button.x;
          y = Event.button.y;

          if (ButtonPressed && IsHit(x, y))
          {
            ButtonEvent = true;
          }

          if (IsHit(x, y))
          {
            State = SDLCONTROL_STATE_ACTIVE_HILIGHT;
          }
          else
          {
            State = SDLCONTROL_STATE_ACTIVE_NORMAL;
          }
          ButtonPressed = false;
          break;

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
        if (ButtonPressed)
        {
          State = SDLCONTROL_STATE_ACTIVE_PRESSED;
        }
        else
        {
          State = SDLCONTROL_STATE_ACTIVE_HILIGHT;
        }
      }
      else
      {
        State = SDLCONTROL_STATE_ACTIVE_NORMAL;
      }
      break;
    }

    default:
      break;

  }

  return ButtonEvent;
}
