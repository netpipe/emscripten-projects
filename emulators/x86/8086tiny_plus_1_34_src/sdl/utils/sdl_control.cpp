/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_control.cpp
 *
 * DESCRIPTION:
 * This module define the virtual base class used for SDL GUI controls.
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


#include "sdl_control.h"

// =============================================================================
// Exported Functions
//

void Draw3DRect(SDL_Surface *Surf, SDL_Rect &Rect, unsigned int HiColour, unsigned int LoColour)
{
  SDL_Rect TRect;

  TRect.x = Rect.x;
  TRect.y = Rect.y;
  TRect.w = Rect.w;
  TRect.h = 2;
  SDL_FillRect(Surf, &TRect, HiColour);

  TRect.w = 2;
  TRect.h = Rect.h;
  SDL_FillRect(Surf, &TRect, HiColour);

  TRect.x = Rect.x;
  TRect.y = Rect.y+Rect.h-1;
  TRect.w = Rect.w;
  TRect.h = 1;
  SDL_FillRect(Surf, &TRect, LoColour);

  TRect.x = Rect.x+1;
  TRect.y = Rect.y+Rect.h-2;
  TRect.w = Rect.w-2;
  TRect.h = 1;
  SDL_FillRect(Surf, &TRect, LoColour);

  TRect.x = Rect.x+Rect.w-1;
  TRect.y = Rect.y;
  TRect.w = 1;
  TRect.h = Rect.h;
  SDL_FillRect(Surf, &TRect, LoColour);

  TRect.x = Rect.x+Rect.w-2;
  TRect.y = Rect.y+1;
  TRect.w = 1;
  TRect.h = Rect.h-2;
  SDL_FillRect(Surf, &TRect, LoColour);

}

// =============================================================================
// Class: SDLControl_t
//

SDLControl_t::SDLControl_t(int ControlId, struct SDLControlData_t &Data)
{
  //
  // Store and set initial control data.
  //
  Id = ControlId;
  ControlData = Data;
  Next = NULL;
  Prev = NULL;

  IsActive = false;
  State = SDLCONTROL_STATE_INACTIVE;
}

SDLControl_t::~SDLControl_t()
{
}

void SDLControl_t::Reset(void)
{
  //
  // Reset the control state
  //
  if (IsActive)
  {
    State = SDLCONTROL_STATE_ACTIVE_NORMAL;
  }
  else
  {
    State = SDLCONTROL_STATE_INACTIVE;
  }
}

int SDLControl_t::GetId(void)
{
  return Id;
}


void SDLControl_t::Insert(SDLControl_t *NextCtrl)
{
  if (Next != NULL)
  {
    Next->Prev = NextCtrl;
  }

  NextCtrl->Next = Next;
  NextCtrl->Prev = this;

  Next = NextCtrl;
}

void SDLControl_t::Remove(void)
{
  if (Prev != NULL)
  {
    Prev->Next = Next;
  }

  if (Next != NULL)
  {
    Next->Prev = Prev;
  }
}

SDLControl_t *SDLControl_t::GetNext(void)
{
  return Next;
}


void SDLControl_t::SetPosition(const SDL_Rect &Rect)
{
  ControlData.Rect = Rect;
}

void SDLControl_t::SetImage(SDLControlState_t State, struct SDLControlImageData_t *Data)
{
  ControlData.ImageData[State] = *Data;
}

void SDLControl_t::SetActive(bool Active)
{
  IsActive = Active;
  if (!IsActive)
  {
    State = SDLCONTROL_STATE_INACTIVE;
  }
  else
  {
    State = SDLCONTROL_STATE_ACTIVE_NORMAL;
  }
}

bool SDLControl_t::HandleEvent(SDL_Event &Event)
{
  return false;
}

bool SDLControl_t::IsHit(int x, int y)
{
  return ((x >= ControlData.Rect.x) && (x < (ControlData.Rect.x + ControlData.Rect.w)) &&
          (y >= ControlData.Rect.y) && (y < (ControlData.Rect.y + ControlData.Rect.h)));
}

