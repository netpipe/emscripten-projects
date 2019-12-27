/* =============================================================================
 * PROGRAM:  library
 * FILENAME: sdl_listbox.cpp
 *
 * DESCRIPTION:
 * This module defines the SDLListBox Control class.
 * This class is derived from the SDLControl class and defines a list box
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

#include "sdl_listbox.h"
#include "sdl_simple_text.h"

SDLListBox_t::SDLListBox_t(int ControlId, struct SDLControlData_t &Data)
  : SDLControl_t(ControlId, Data)
{
  ListHead = NULL;
  ListTail = NULL;
  LineCount = 0;
  SelIndex = -1;
  VScrollActive = false;

  LineSelectTick = 0;
  LastEventIsDoubleClick = false;

  SDLControlData_t ScrollData = ControlData;
  ScrollData.Rect.x = ControlData.Rect.x + ControlData.Rect.w - 20;
  ScrollData.Rect.y = ControlData.Rect.y;
  ScrollData.Rect.w = 20;
  ScrollData.Rect.h = ControlData.Rect.h;
  VScroll = new SDLVScroll_t(1001, ScrollData);
  VScroll->SetStep(16);
}

SDLListBox_t::~SDLListBox_t()
{
  delete VScroll;
  Clear();
}

void SDLListBox_t::SetActive(bool Active)
{
  SDLControl_t::SetActive(Active);
  VScroll->SetActive(Active);
}

void SDLListBox_t::Draw(SDL_Surface *Surf)
{
  int text_w, text_h;

  SDL_FillRect(Surf, &ControlData.Rect, ControlData.ImageData[State].FaceColour);

  Draw3DRect(Surf,
             ControlData.Rect,
             ControlData.ImageData[State].LoColour,
             ControlData.ImageData[State].HiColour);

  LineNode_t *Node = ListHead;
  int LineYOffset = 2;
  int LineIdx = 0;

  if (VScrollActive)
  {
    LineYOffset -= VScroll->GetValue();
  }

  while (LineYOffset < -6)
  {
    SDLTEXT_GetSize(Node->Text, text_w, text_h, strlen(Node->Text));

    Node = Node->Next;
    LineYOffset += text_h;
    LineIdx++;
  }

  SDL_Rect ClipRect;
  SDL_Rect OldClipRect;
  SDL_Rect HRect;

  ClipRect.x = ControlData.Rect.x+2;
  ClipRect.y = ControlData.Rect.y+2;
  if (VScrollActive)
  {
    ClipRect.w = ControlData.Rect.w - 24;
  }
  else
  {
    ClipRect.w = ControlData.Rect.w - 4;
  }
  ClipRect.h = ControlData.Rect.h-4;

  SDL_GetClipRect(Surf, &OldClipRect);
  SDL_SetClipRect(Surf, &ClipRect);

  while ((Node != NULL) && (LineYOffset < ControlData.Rect.h))
  {
    SDLTEXT_GetSize(Node->Text, text_w, text_h, strlen(Node->Text));

    if (LineIdx == SelIndex)
    {
      HRect.x = ControlData.Rect.x+2;
      HRect.y = ControlData.Rect.y + LineYOffset;
      HRect.w = ControlData.Rect.w;
      HRect.h = text_h;

      SDL_FillRect(Surf, &HRect, ControlData.ImageData[SDLCONTROL_STATE_ACTIVE_PRESSED].FaceColour);
    }
    else if (LineIdx == HilightIndex)
    {
      HRect.x = ControlData.Rect.x+2;
      HRect.y = ControlData.Rect.y + LineYOffset;
      HRect.w = ControlData.Rect.w;
      HRect.h = text_h;

      SDL_FillRect(Surf, &HRect, ControlData.ImageData[SDLCONTROL_STATE_ACTIVE_HILIGHT].FaceColour);
    }

    SDLTEXT_DrawString(Surf, ControlData.Rect.x+4, ControlData.Rect.y + LineYOffset, 0x00000000, Node->Text);

    Node = Node->Next;
    LineYOffset += text_h;
    LineIdx++;
  }

  SDL_SetClipRect(Surf, &OldClipRect);

  if (VScrollActive)
  {
    VScroll->Draw(Surf);
  }
}

bool SDLListBox_t::HandleEvent(SDL_Event &Event)
{
  bool ButtonEvent = false;
  int x, y;
  int NewSelIndex;

  if (!IsActive)
  {
    return false;
  }

  if (VScrollActive)
  {
    if (VScroll->HandleEvent(Event))
    {
      return false;
    }
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

          if (IsListHit(x, y))
          {
            LastEventIsDoubleClick = false;

            NewSelIndex = ItemUnderMouse(x, y);
            if (NewSelIndex == SelIndex)
            {
              int Ticks = SDL_GetTicks() - LineSelectTick;
              if (Ticks < 250)
              {
                LastEventIsDoubleClick = true;
              }
            }

            ButtonEvent = true;
            SelIndex = NewSelIndex;
            LineSelectTick = SDL_GetTicks();
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

      if (IsListHit(x, y))
      {
        HilightIndex = ItemUnderMouse(x, y);
      }
      else
      {
        HilightIndex = -1;
      }
      break;
    }

    default:
      break;

  }

  return ButtonEvent;
}

void SDLListBox_t::Clear(void)
{
  LineNode_t *Tmp;
  while (ListHead != NULL)
  {
    Tmp = ListHead;
    ListHead = ListHead->Next;

    delete[] Tmp->Text;
    delete Tmp;
  }

  LineCount = 0;
  SelIndex = -1;
}

void SDLListBox_t::AddLine(const char *Text)
{
  // Allocate a new line node and copy the text
  LineNode_t *NewNode = new LineNode_t;
  int SLen = strlen(Text);
  NewNode->Text = new char[SLen+1];
  strcpy(NewNode->Text, Text);

  // Add to the linked list of line nodes.
  if (ListHead == NULL)
  {
    NewNode->Prev = NULL;
    NewNode->Next = NULL;
    ListHead = NewNode;
    ListTail = NewNode;
  }
  else
  {
    ListTail->Next = NewNode;
    NewNode->Prev = ListTail;
    NewNode->Next = NULL;
    ListTail = NewNode;
  }

  LineCount++;

  CalcDisplaySize();
}

void SDLListBox_t::SetSelText(const char *Text)
{
  if (SelIndex != -1)
  {
    LineNode_t *Pos = ListHead;
    int Line = 0;

    while ((Pos != NULL) && (Line != SelIndex))
    {
      Pos = Pos->Next;
      Line++;
    }

    if (Pos == NULL)
    {
      return;
    }

    delete[] Pos->Text;
    Pos->Text = new char[strlen(Text)+1];
    strcpy(Pos->Text, Text);
  }
}

int SDLListBox_t::GetLineCount(void)
{
  return LineCount;
}

bool SDLListBox_t::IsDoubleClickEvent(void)
{
  return LastEventIsDoubleClick;
}


int SDLListBox_t::GetSelIdx(void)
{
  return SelIndex;
}

void SDLListBox_t::SetSelIdx(int Idx)
{
  if ((Idx < 0) || (Idx > LineCount))
  {
    SelIndex = -1; // No selection
  }
  else
  {
    SelIndex = Idx;
  }
}

void SDLListBox_t::DeleteSel(void)
{
  if (SelIndex != -1)
  {
    LineNode_t *Pos = ListHead;
    int Line = 0;

    while ((Pos != NULL) && (Line != SelIndex))
    {
      Pos = Pos->Next;
      Line++;
    }

    if (Pos == NULL)
    {
      return;
    }

    if (Pos->Prev == NULL)
    {
      ListHead = Pos->Next;
    }
    else
    {
      Pos->Prev->Next = Pos->Next;
    }

    if (Pos->Next == NULL)
    {
      ListTail = Pos->Prev;
      SelIndex--;
    }
    else
    {
      Pos->Next->Prev = Pos->Prev;
    }

    delete[] Pos->Text;
    delete Pos;
  }
}

char *SDLListBox_t::GetText(int Idx)
{
  if ((Idx < 0) || (Idx >= LineCount))
  {
    return NULL;
  }

  LineNode_t *Pos = ListHead;
  int Line = 0;

  while ((Pos != NULL) && (Line != Idx))
  {
    Pos = Pos->Next;
    Line++;
  }

  if (Pos == NULL)
  {
    return NULL;
  }

  return Pos->Text;
}


bool SDLListBox_t::IsListHit(int x, int y)
{
  bool HitX;
  bool HitY;

  if (VScrollActive)
  {
    HitX = ((x >= ControlData.Rect.x+2)) &&
            (x < ControlData.Rect.x+ControlData.Rect.w - 24);
  }
  else
  {
    HitX = ((x >= ControlData.Rect.x+2)) &&
            (x < ControlData.Rect.x+ControlData.Rect.w - 4);
  }

  HitY = ((y >= ControlData.Rect.y+2)) &&
          (y < ControlData.Rect.y+ControlData.Rect.h - 4);

  return HitX && HitY;
}

int SDLListBox_t::ItemUnderMouse(int x, int y)
{
  int YOffset = 0;
  if (VScrollActive)
  {
    YOffset = VScroll->GetValue();
  }

  int Line = (y + YOffset - (ControlData.Rect.y + 2)) / 16;

  if (Line > LineCount)
  {
    Line = -1;
  }

  return Line;
}

void SDLListBox_t::CalcDisplaySize(void)
{
  LineNode_t *Node = ListHead;
  int DispWidth = 0;
  int DispHeight = 0;
  int text_w;
  int text_h;

  while (Node != NULL)
  {
    SDLTEXT_GetSize(Node->Text, text_w, text_h, strlen(Node->Text));

    if (text_w > DispWidth) DispWidth = text_w;
    DispHeight += text_h;

    Node = Node->Next;
  }

  VScrollActive = (DispHeight > ControlData.Rect.h-4);
  if (VScrollActive)
  {
    VScroll->SetRange(0, DispHeight - (ControlData.Rect.h-4));
  }
}
