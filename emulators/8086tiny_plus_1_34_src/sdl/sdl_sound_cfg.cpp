// =============================================================================
// File: sdl_sound_cfg.cpp
//
// Description:
// SDL sound configuration settings and dialog.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include "sdl_sound_cfg.h"

#include "sdl_simple_text.h"
#include "sdl_control_list.h"
#include "sdl_button.h"
#include "sdl_checkbox.h"
#include "sdl_edit.h"
#include "sdl_slider.h"
#include <string.h>

// =============================================================================
// Exported Variables
//

#define ID_BUTTON_OK        101
#define ID_BUTTON_CANCEL    102
#define ID_EDIT_SAMPLE_RATE 103
#define ID_SLIDER_VOLUME    104
#define ID_CHECKBOX_ENABLE  105

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

static SDLControlData_t ChkBoxData_Enable =
{
  { 30, 30, 20, 20 },
  "Enable Sound",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_SampleRate =
{
  { 140, 70, 160, 24 },
  "Sample Rate",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t SliderData_Volume =
{
  { 140, 110, 160, 24 },
  "Volume",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static int TmpVolumePercent;

// =============================================================================
// Exported Variables
//

bool SoundEnabled = true;

int AudioSampleRate = 48000;

int VolumePercent = 100;
int VolumeSample = 127;

// =============================================================================
// Exported Functions
//

bool SNDCFG_Read(FILE *fp)
{
  char Line[256];
  int tmp;

  fgets(Line, 256, fp);
  if (strncmp(Line, "[SOUND_ENABLE]", 14) != 0) return 0;

  fgets(Line, 256, fp);
  sscanf(Line, "%d\n", &tmp);
  SoundEnabled = (tmp != 0);

  fgets(Line, 256, fp);
  if (strncmp(Line, "[SOUND_SAMPLE_RATE]", 19) != 0) return 0;
  fgets(Line, 256, fp);
  sscanf(Line, "%d\n", &AudioSampleRate);

  fgets(Line, 256, fp);
  if (strncmp(Line, "[SOUND_VOLUME]", 14) != 0) return 0;
  fgets(Line, 256, fp);
  sscanf(Line, "%d\n", &VolumePercent);
  VolumeSample = (VolumePercent * 127) / 100;

  return true;
}

bool SNDCFG_Write(FILE *fp)
{
  fprintf(fp, "[SOUND_ENABLE]\n");
  fprintf(fp, "%d\n", SoundEnabled ? 1 : 0);
  fprintf(fp, "[SOUND_SAMPLE_RATE]\n");
  fprintf(fp, "%d\n", AudioSampleRate);
  fprintf(fp, "[SOUND_VOLUME]\n");
  fprintf(fp, "%d\n", VolumePercent);

  return true;
}

bool SNDCFG_Dialog(void)
{
  SDLControlList_t Controls;
  bool SoundUpdated = false;
  int EditLen;
  char Buffer[16];

  Controls.AddControl(new SDLButton_t(ID_BUTTON_OK, ButtonData_OK));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_CANCEL, ButtonData_Cancel));
  Controls.AddControl(new SDLCheckBox_t(ID_CHECKBOX_ENABLE, ChkBoxData_Enable, false, NULL));
  Controls.AddControl(new SDLEdit_t(ID_EDIT_SAMPLE_RATE, EditData_SampleRate, 16));
  Controls.AddControl(new SDLSlider_t(ID_SLIDER_VOLUME, SliderData_Volume));

  Controls.GetControl(ID_BUTTON_OK)->SetActive(true);
  Controls.GetControl(ID_BUTTON_CANCEL)->SetActive(true);

  SDLCheckBox_t *EnableChkBox = (SDLCheckBox_t *) Controls.GetControl(ID_CHECKBOX_ENABLE);
  EnableChkBox->SetActive(true);
  EnableChkBox->SetChecked(SoundEnabled);

  SDLSlider_t *SliderControl = (SDLSlider_t *) Controls.GetControl(ID_SLIDER_VOLUME);
  SliderControl->SetActive(true);
  SliderControl->SetRange(0, 100);
  SliderControl->SetValue(VolumePercent);
  TmpVolumePercent = VolumePercent;

  SDLEdit_t *EditControl =  (SDLEdit_t *) Controls.GetControl(ID_EDIT_SAMPLE_RATE);
  EditControl->SetActive(true);
  sprintf(Buffer, "%d", AudioSampleRate);
  EditControl->SetText(Buffer);


  SDL_Window *DialogWindow =
    SDL_CreateWindow(
      "Configure sound",
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
    SDLTEXT_DrawString(Surf, 20, 74, 0x00000000, "Sample Rate:");
    sprintf(Buffer, "Volume: %d", TmpVolumePercent);
    SDLTEXT_DrawString(Surf, 20, 114, 0x00000000, Buffer);
    Controls.Draw(Surf);
    SDL_UpdateWindowSurface(DialogWindow);
    SDL_WaitEvent(&Event);

    if (IsEventForWindow(Event, SDL_GetWindowID(DialogWindow)))
    {
      ControlId = Controls.HandleEvent(Event);

      switch (ControlId)
      {
        case ID_SLIDER_VOLUME:
          TmpVolumePercent = SliderControl->GetValue();
          break;

        case ID_BUTTON_OK:
          Done = 1;
          SoundUpdated = true;

          SoundEnabled = EnableChkBox->GetCheck();

          EditLen = 16;
          EditControl->GetText(Buffer, &EditLen);
          AudioSampleRate = atoi(Buffer);
          if (AudioSampleRate == 0) AudioSampleRate = 48000;

          VolumePercent = TmpVolumePercent;
          VolumeSample = (127 * TmpVolumePercent) / 100;
          break;

        case ID_BUTTON_CANCEL:
          Done = 1;
          break;
      }
    }

  }

  SDL_DestroyWindow(DialogWindow);

  return SoundUpdated;
}

