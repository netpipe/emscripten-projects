/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_vscroll.cpp
 *
 * DESCRIPTION:
 * This module defines the SDLVScroll Control class.
 * This class is derived from the SDLControl class and defines a vertical
 * scrollbar control for use in a SDLControlList.
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

#include "sdl_vscroll.h"
#include "sdl_simple_text.h"

#define DEFAULT_THUMB_SIZE 20

static const char *UpButtonText = "\036";
static const char *DownButtonText = "\037";

SDLVScroll_t::SDLVScroll_t(int ControlId, struct SDLControlData_t &Data)
  : SDLControl_t(ControlId, Data)
{
  ThumbPressed = false;
  MinValue = 0;
  MaxValue = 100;
  CurrentValue = 0;

  Range = MaxValue - MinValue;
  ThumbSize = DEFAULT_THUMB_SIZE;
  SlideTop = ControlData.Rect.y + 20;
  SlideSize = ControlData.Rect.h - 40;
  SlideRange = SlideSize - ThumbSize;
  ThumbPos = ValueToThumbPos(CurrentValue);
  StepSize = 1;

  SDLControlData_t ButtonData = Data;
  ButtonData.Rect.h = 20;
  ButtonData.Text = UpButtonText;
  UpButton = new SDLButton_t(1001, ButtonData);

  ButtonData.Rect.y = Data.Rect.y + Data.Rect.h - 20;
  ButtonData.Text = DownButtonText;
  DownButton = new SDLButton_t(1002, ButtonData);
}

SDLVScroll_t::~SDLVScroll_t()
{
  delete UpButton;
  delete DownButton;
}

void SDLVScroll_t::SetActive(bool Active)
{
  SDLControl_t::SetActive(Active);
  UpButton->SetActive(Active);
  DownButton->SetActive(Active);
}

void SDLVScroll_t::Draw(SDL_Surface *Surf)
{
  int text_w, text_h;
  SDLTEXT_GetSize(ControlData.Text, text_w, text_h, strlen(ControlData.Text));

  SDL_Rect TRect;

  TRect.x = ControlData.Rect.x;
  TRect.y = SlideTop;
  TRect.w = ControlData.Rect.w;
  TRect.h = SlideSize;

  Draw3DRect(Surf,
             TRect,
             ControlData.ImageData[State].LoColour,
             ControlData.ImageData[State].HiColour);

  TRect.y = ThumbPos-ThumbSize/2;
  TRect.h = ThumbSize;

  SDL_FillRect(Surf, &TRect, ControlData.ImageData[State].FaceColour);

  Draw3DRect(Surf,
             TRect,
             ControlData.ImageData[State].HiColour,
             ControlData.ImageData[State].LoColour);

  UpButton->Draw(Surf);
  DownButton->Draw(Surf);
}

bool SDLVScroll_t::HandleEvent(SDL_Event &Event)
{
  bool ButtonEvent = false;
  int x, y;

  if (!IsActive)
  {
    return false;
  }

  if (UpButton->HandleEvent(Event))
  {
    CurrentValue -= StepSize;
    if (CurrentValue < MinValue) CurrentValue = MinValue;
    ThumbPos = ValueToThumbPos(CurrentValue);
    return true;
  }

  if (DownButton->HandleEvent(Event))
  {
    CurrentValue += StepSize;
    if (CurrentValue > MaxValue) CurrentValue = MaxValue;
    ThumbPos = ValueToThumbPos(CurrentValue);
    return true;
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
            CurrentValue = ThumbPosToValue(y);
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
        CurrentValue = ThumbPosToValue(y);
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

void SDLVScroll_t::SetRange(int Min, int Max)
{
  MinValue = Min;
  MaxValue = Max;

  if (CurrentValue < MinValue) CurrentValue = MinValue;
  if (CurrentValue > MaxValue) CurrentValue = MaxValue;

  Range = MaxValue - MinValue;
  ThumbPos = ValueToThumbPos(CurrentValue);
}

void SDLVScroll_t::SetValue(int Val)
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

  ThumbPos = ValueToThumbPos(CurrentValue);
}

void SDLVScroll_t::SetStep(int Step)
{
  StepSize = Step;
}

int SDLVScroll_t::GetValue(void)
{
  return CurrentValue;
}

bool SDLVScroll_t::IsThumbHit(int mx, int my)
{
  return ((mx >= ControlData.Rect.x) && (mx < ControlData.Rect.x + ControlData.Rect.w) &&
          (my >= (ThumbPos - ThumbSize/2)) && (my < (ThumbPos + ThumbSize /2)));
}

int SDLVScroll_t::ThumbPosToValue(int my)
{
  if (my <= (SlideTop + ThumbSize/2))
  {
    return MinValue;
  }
  else if (my >= (SlideTop + ThumbSize/2 + SlideRange))
  {
    return MaxValue;
  }

  return (my - (SlideTop + (ThumbSize/2))) * Range / SlideRange;
}

int SDLVScroll_t::ValueToThumbPos(int Value)
{
  return SlideTop + (ThumbSize / 2) + ((CurrentValue - MinValue) * SlideRange) / Range;
}
