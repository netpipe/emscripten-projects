// =============================================================================
// File: sdl_cpu_speed_dialog.h
//
// Description:
// SDL CPU speed dialog.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include <stdio.h>

#include "sdl_cpu_speed_dialog.h"

#include "sdl_simple_text.h"
#include "sdl_control_list.h"
#include "sdl_button.h"
#include "sdl_edit.h"

#define ID_BUTTON_OK      101
#define ID_BUTTON_CANCEL  102
#define ID_EDIT_CPU_SPEED 103

static SDLControlData_t ButtonData_OK =
{
  { 250, 160, 60, 24 },
  "OK",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_Cancel =
{
  { 10, 160, 60, 24 },
  "Cancel",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_CPUSpeed =
{
  { 20, 90, 280, 24 },
  "CPU Speed",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

bool CPU_SPEED_Dialog(int &CPUSpeed)
{
  SDLControlList_t Controls;
  bool SpeedUpdated = false;
  int EditLen;
  char Buffer[16];

  Controls.AddControl(new SDLButton_t(ID_BUTTON_OK, ButtonData_OK));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_CANCEL, ButtonData_Cancel));
  Controls.AddControl(new SDLEdit_t(ID_EDIT_CPU_SPEED, EditData_CPUSpeed, 16));


  Controls.GetControl(ID_BUTTON_OK)->SetActive(true);
  Controls.GetControl(ID_BUTTON_CANCEL)->SetActive(true);

  SDLEdit_t *EditControl =  (SDLEdit_t *) Controls.GetControl(ID_EDIT_CPU_SPEED);
  EditControl->SetActive(true);
  sprintf(Buffer, "%d", CPUSpeed);
  EditControl->SetText(Buffer);


  SDL_Window *DialogWindow =
    SDL_CreateWindow(
      "Set CPU Speed",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      320, 200,
      0);

  SDL_Surface *Surf = SDL_GetWindowSurface(DialogWindow);

  SDL_Event Event;
  int Done;
  int ControlId;
  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = 0;
  DRect.w = 320;
  DRect.h = 200;

  Done = 0;

  while (!Done)
  {
    SDL_FillRect(Surf, &DRect, 0x00C0C0C0);
    SDLTEXT_DrawString(Surf, 76, 60, 0x00000000, "Enter CPU Speed (Hz)");
    Controls.Draw(Surf);
    SDL_UpdateWindowSurface(DialogWindow);

    SDL_WaitEvent(&Event);

    if (IsEventForWindow(Event, SDL_GetWindowID(DialogWindow)))
    {
      ControlId = Controls.HandleEvent(Event);

      switch (ControlId)
      {
        case ID_BUTTON_OK:
          Done = 1;
          SpeedUpdated = true;
          EditLen = 16;
          EditControl->GetText(Buffer, &EditLen);
          CPUSpeed = atoi(Buffer);
          if (CPUSpeed == 0) CPUSpeed = 1000000;
          break;

        case ID_BUTTON_CANCEL:
          Done = 1;
          break;
      }
    }

  }

  SDL_DestroyWindow(DialogWindow);

  return SpeedUpdated;
}
