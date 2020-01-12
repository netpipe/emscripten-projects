/* =============================================================================
 * PROGRAM:  library
 * FILENAME: gl_control_list.cpp
 *
 * DESCRIPTION:
 * This module define the class used for OpenGL GUI control lists.
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

#include "sdl_control_list.h"

// ============================================================================
// Exported Functions
//

bool IsEventForWindow(SDL_Event &Event, Uint32 WindowId)
{
  switch (Event.type)
  {
    case SDL_WINDOWEVENT:
    {
      if (Event.window.windowID != WindowId) return false;
      break;
    }

    case SDL_KEYDOWN:
    case SDL_KEYUP:
      if (Event.key.windowID != WindowId) return false;
      break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    {
      if (Event.button.windowID != WindowId) return false;
      break;
    }

    case SDL_MOUSEMOTION:
    {
      if (Event.motion.windowID != WindowId) return false;
      break;
    }

    default:
      break;
  }

  return true;
}

// =============================================================================
// Class: SDLControlList_t
//

SDLControlList_t::SDLControlList_t()
{
  Head = NULL;
}

SDLControlList_t::~SDLControlList_t()
{
  SDLControl_t *Ptr;

  while (Head != NULL)
  {
    //
    // Remove the head of the list form the list of controls
    //
    Ptr = Head;
    RemoveControl(Ptr);

    //
    // Delete the control removed.
    //
    delete Ptr;
  }

}

void SDLControlList_t::AddControl(SDLControl_t *Control)
{
  if (Head == NULL)
  {
    Head = Control;
  }
  else
  {
    Head->Insert(Control);
  }
}

void SDLControlList_t::RemoveControl(SDLControl_t *Control)
{
  if (Control == Head)
  {
    Head = Head->GetNext();
  }

  Control->Remove();
}

void SDLControlList_t::Reset(void)
{
  SDLControl_t *Control = Head;

  while (Control != NULL)
  {
    Control->Reset();

    Control = Control->GetNext();
  }

}

SDLControl_t *SDLControlList_t::GetControl(int Id)
{
  SDLControl_t *Control = Head;

  while ((Control != NULL) && (Control->GetId() != Id))
  {
    Control = Control->GetNext();
  }

  return Control;
}

int SDLControlList_t::HandleEvent(SDL_Event &Event)
{
  SDLControl_t *Control = Head;

  int EventControlId = 0;

  while (Control != NULL)
  {
    if (Control->HandleEvent(Event))
    {
      EventControlId = Control->GetId();
    }

    Control = Control->GetNext();
  }

  return EventControlId;
}

void SDLControlList_t::Draw(SDL_Surface *Surf)
{
  SDLControl_t *Control = Head;

  while (Control != NULL)
  {
    Control->Draw(Surf);

    Control = Control->GetNext();
  }
}
