/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_checkbox.cpp
 *
 * DESCRIPTION:
 * This class defines the SDLCheckBox control class
 * This class is derived from the SDLControl class and defines a check box
 * control for use in a SDlControlList.
 * This class also implements exclusive sets so that radio button functionality
 * is also provided.
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


#include "sdl_checkbox.h"
#include "sdl_simple_text.h"

static const unsigned char CheckString[2] = { 251, 0 };

SDLCheckBox_t::SDLCheckBox_t(int ControlId, struct SDLControlData_t Data, bool Exclusive, SDLCheckBox_t *NextInSet)
  : SDLControl_t(ControlId, Data)
{
  SDLCheckBox_t *Ptr;

  IsChecked = false;
  IsExclusive = Exclusive;
  ExclusiveSetStart = this;
  ExclusiveSetNext  = NextInSet;

  Ptr = NextInSet;
  while (Ptr != NULL)
  {
    Ptr->ExclusiveSetStart = this;

    Ptr = Ptr->ExclusiveSetNext;
  }
}

SDLCheckBox_t::~SDLCheckBox_t()
{
}

void SDLCheckBox_t::Draw(SDL_Surface *Surf)
{

  //
  // Draw the checkbox background
  //

  SDL_FillRect(Surf, &ControlData.Rect, ControlData.ImageData[State].FaceColour);

  Draw3DRect(Surf,
             ControlData.Rect,
             ControlData.ImageData[State].LoColour,
             ControlData.ImageData[State].HiColour);


  SDLTEXT_DrawString(
     Surf,
     ControlData.Rect.x + ControlData.Rect.w + 8,
     ControlData.Rect.y + ControlData.Rect.h/2 - 8,
     0x00000000,
     ControlData.Text);

  if (IsChecked)
  {
    SDLTEXT_DrawString(
       Surf,
       ControlData.Rect.x + ControlData.Rect.w / 2 - 4,
       ControlData.Rect.y + ControlData.Rect.h / 2 - 8,
       0x00000000,
       (char *) CheckString);
  }

}

bool SDLCheckBox_t::HandleEvent(SDL_Event &Event)
{
  SDLCheckBox_t *Ptr;
  bool ButtonEvent = false;
  int x, y;

  if (!IsActive)
  {
    return false;
  }

  switch (Event.type)
  {
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
        {
          x = Event.button.x;
          y = Event.button.y;

          //
          // Check that the mouse is still over the control when the button
          // is released.
          //
          if (ButtonPressed && IsHit(x, y))
          {
            ButtonEvent = true;
            if (IsExclusive)
            {
              IsChecked = true;
              Ptr = ExclusiveSetStart;
              while (Ptr != NULL)
              {
                if (Ptr != this)
                {
                  Ptr->IsChecked = false;
                }

                Ptr = Ptr->ExclusiveSetNext;
              }
            }
            else
            {
              IsChecked   = !IsChecked;
            }
          }

          //
          // Update the hilight state according to whether the mouse is still over
          // the control
          //
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
        }

        default:
          break;
      }
    }

    default:
      break;
  }

  return ButtonEvent;
}

void SDLCheckBox_t::SetChecked(bool Checked)
{
  IsChecked = Checked;
}

bool SDLCheckBox_t::GetCheck(void)
{
  return IsChecked;
}
