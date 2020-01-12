/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_slider.cpp
 *
 * DESCRIPTION:
 * This module defines the SDLSlider Control class.
 * This class is derived from the SDLControl class and defines a slider
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

#include "sdl_slider.h"
#include "sdl_simple_text.h"

#define THUMB_WIDTH 12

SDLSlider_t::SDLSlider_t(int ControlId, struct SDLControlData_t &Data)
  : SDLControl_t(ControlId, Data)
{
  ThumbPressed = false;
  MinValue = 0;
  MaxValue = 100;
  CurrentValue = 0;

  Range = MaxValue - MinValue;
  SlideRange = ControlData.Rect.w - THUMB_WIDTH;
  ThumbPos = ValueToThumbPos(CurrentValue);
}

SDLSlider_t::~SDLSlider_t()
{
}

void SDLSlider_t::Draw(SDL_Surface *Surf)
{
  SDL_Rect TRect;

  TRect.x = ControlData.Rect.x;
  TRect.y = ControlData.Rect.y + (ControlData.Rect.h / 2) - 2;
  TRect.w = ControlData.Rect.w;
  TRect.h = 4;

  Draw3DRect(Surf,
             TRect,
             ControlData.ImageData[State].LoColour,
             ControlData.ImageData[State].HiColour);

  TRect.x = ThumbPos - (THUMB_WIDTH / 2);
  TRect.y = ControlData.Rect.y;
  TRect.w = THUMB_WIDTH;
  TRect.h = ControlData.Rect.h;

  SDL_FillRect(Surf, &TRect, ControlData.ImageData[State].FaceColour);

  Draw3DRect(Surf,
             TRect,
             ControlData.ImageData[State].HiColour,
             ControlData.ImageData[State].LoColour);
}

bool SDLSlider_t::HandleEvent(SDL_Event &Event)
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

          if (IsThumbHit(x, y))
          {
            State = SDLCONTROL_STATE_ACTIVE_PRESSED;
            ThumbPressed = true;
          }
          else if (IsHit(x, y))
          {
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

          if (ThumbPressed)
          {
            ThumbPressed = false;
            CurrentValue = ThumbPosToValue(x);
            ThumbPos = ValueToThumbPos(CurrentValue);
            ButtonEvent = true;
          }

          if (IsThumbHit(x, y))
          {
            State = SDLCONTROL_STATE_ACTIVE_HILIGHT;
          }
          else
          {
            State = SDLCONTROL_STATE_ACTIVE_NORMAL;
          }
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

      if (ThumbPressed)
      {
        CurrentValue = ThumbPosToValue(x);
        ThumbPos = ValueToThumbPos(CurrentValue);
        ButtonEvent = true;
      }
      else if (IsThumbHit(x, y))
      {
        State = SDLCONTROL_STATE_ACTIVE_HILIGHT;
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

void SDLSlider_t::SetRange(int Min, int Max)
{
  MinValue = Min;
  MaxValue = Max;

  if (CurrentValue < MinValue) CurrentValue = MinValue;
  if (CurrentValue > MaxValue) CurrentValue = MaxValue;

  Range = MaxValue - MinValue;
  ThumbPos = ValueToThumbPos(CurrentValue);
}

void SDLSlider_t::SetValue(int Val)
{
  if (Val < MinValue)
  {
    CurrentValue = MinValue;
  }
  else if (Val > MaxValue)
  {
    CurrentValue = MaxValue;
  }
  else
  {
    CurrentValue = Val;
  }

  ThumbPos = ControlData.Rect.x + (THUMB_WIDTH / 2) + ((CurrentValue - MinValue) * SlideRange) / Range;
}

int SDLSlider_t::GetValue(void)
{
  return CurrentValue;
}

bool SDLSlider_t::IsThumbHit(int mx, int my)
{
  return ((mx >= (ThumbPos - THUMB_WIDTH/2)) && (mx < (ThumbPos + THUMB_WIDTH/2)) &&
          (my >= ControlData.Rect.y) && (my < ControlData.Rect.y + ControlData.Rect.h));
}

int SDLSlider_t::ThumbPosToValue(int mx)
{
  if (mx <= (ControlData.Rect.x + THUMB_WIDTH/2))
  {
    return MinValue;
  }
  else  if (mx >= (ControlData.Rect.x + ControlData.Rect.w - THUMB_WIDTH / 2))
  {
    return MaxValue;
  }

  return (mx - (ControlData.Rect.x + (THUMB_WIDTH/2))) * Range / SlideRange;
}

int SDLSlider_t::ValueToThumbPos(int Value)
{
  return ControlData.Rect.x + (THUMB_WIDTH / 2) + ((CurrentValue - MinValue) * SlideRange) / Range;
}
