// =============================================================================
// File: sdl_serial_cfg_dialog.h
//
// Description:
// SDL serial port configuration dialog.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include <stdio.h>

#include "sdl_serial_cfg_dialog.h"
#include "serial_emulation.h"

#include "sdl_simple_text.h"
#include "sdl_control_list.h"
#include "sdl_button.h"
#include "sdl_edit.h"

#define ID_BUTTON_OK        101
#define ID_BUTTON_CANCEL    102

#define ID_BUTTON_COM1_SELDOWN 201
#define ID_BUTTON_COM1_SELUP   202
#define ID_EDIT_COM1_TCPADDR   203
#define ID_EDIT_COM1_TCPPORT   204
#define ID_EDIT_COM1_COMNAME   205

#define ID_BUTTON_COM2_SELDOWN 301
#define ID_BUTTON_COM2_SELUP   302
#define ID_EDIT_COM2_TCPADDR   303
#define ID_EDIT_COM2_TCPPORT   304
#define ID_EDIT_COM2_COMNAME   305

#define ID_BUTTON_COM3_SELDOWN 401
#define ID_BUTTON_COM3_SELUP   402
#define ID_EDIT_COM3_TCPADDR   403
#define ID_EDIT_COM3_TCPPORT   404
#define ID_EDIT_COM3_COMNAME   405

#define ID_BUTTON_COM4_SELDOWN 501
#define ID_BUTTON_COM4_SELUP   502
#define ID_EDIT_COM4_TCPADDR   503
#define ID_EDIT_COM4_TCPPORT   504
#define ID_EDIT_COM4_COMNAME   505

static const char *SerMappingStr[] =
{
  "Unused",
  "Mouse",
  "TCP Server",
  "TCP Client",
  "COM Port"
};

static SDLControlData_t ButtonData_OK =
{
  { 250, 440, 60, 24 },
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
  { 10, 440, 60, 24 },
  "Cancel",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_COM1SelDown =
{
  { 20, 34, 24, 24 },
  "\021",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_COM1SelUp =
{
  { 276, 34, 24, 24 },
  "\020",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM1COMName =
{
  { 100, 62, 190, 20 },
  "COM1 Name",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM1TCPAddr =
{
  { 100, 62, 190, 20 },
  "COM1 TCP Address",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM1TCPPort =
{
  { 100, 86, 190, 20 },
  "COM1 TCP Port",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};


static SDLControlData_t ButtonData_COM2SelDown =
{
  { 20, 138, 24, 24 },
  "\021",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_COM2SelUp =
{
  { 276, 138, 24, 24 },
  "\020",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM2COMName =
{
  { 100, 166, 190, 20 },
  "COM2 Name",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM2TCPAddr =
{
  { 100, 166, 190, 20 },
  "COM2 TCP Address",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM2TCPPort =
{
  { 100, 190, 190, 20 },
  "COM2 TCP Port",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};


static SDLControlData_t ButtonData_COM3SelDown =
{
  { 20, 242, 24, 24 },
  "\021",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_COM3SelUp =
{
  { 276, 242, 24, 24 },
  "\020",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM3COMName =
{
  { 100, 270, 190, 20 },
  "COM3 Name",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM3TCPAddr =
{
  { 100, 270, 190, 20 },
  "COM3 TCP Address",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM3TCPPort =
{
  { 100, 294, 190, 20 },
  "COM3 TCP Port",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};


static SDLControlData_t ButtonData_COM4SelDown =
{
  { 20, 346, 24, 24 },
  "\021",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_COM4SelUp =
{
  { 276, 346, 24, 24 },
  "\020",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM4COMName =
{
  { 100, 376, 190, 20 },
  "COM4 Name",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM4TCPAddr =
{
  { 100, 376, 190, 20 },
  "COM4 TCP Address",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_COM4TCPPort =
{
  { 100, 398, 190, 20 },
  "COM4 TCP Port",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};


// =============================================================================
// Exported Functions
//

void SERIAL_ConfigDialog(void)
{
  SDLControlList_t Controls;
  SDLEdit_t *COMNameEdit[4];
  SDLEdit_t *TCPAddrEdit[4];
  SDLEdit_t *TCPPortEdit[4];
  SerialMapping_t Mapping[4];
  char *COMName[4];
  char *TCPPort[4];
  char *TCPAddress[4];
  char Buffer[32];
  int sx, sy;
  SDL_Rect GroupRect;
  int Port;

  // Create the controls

  Controls.AddControl(new SDLButton_t(ID_BUTTON_OK, ButtonData_OK));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_CANCEL, ButtonData_Cancel));

  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM1_SELDOWN, ButtonData_COM1SelDown));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM1_SELUP,   ButtonData_COM1SelUp));
  COMNameEdit[0] = new SDLEdit_t(ID_EDIT_COM1_COMNAME,     EditData_COM1COMName, 16);
  TCPAddrEdit[0] = new SDLEdit_t(ID_EDIT_COM1_TCPADDR,     EditData_COM1TCPAddr, 16);
  TCPPortEdit[0] = new SDLEdit_t(ID_EDIT_COM1_TCPPORT,     EditData_COM1TCPPort, 16);

  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM2_SELDOWN, ButtonData_COM2SelDown));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM2_SELUP,   ButtonData_COM2SelUp));
  COMNameEdit[1] = new SDLEdit_t(ID_EDIT_COM2_COMNAME,     EditData_COM2COMName, 16);
  TCPAddrEdit[1] = new SDLEdit_t(ID_EDIT_COM2_TCPADDR,     EditData_COM2TCPAddr, 16);
  TCPPortEdit[1] = new SDLEdit_t(ID_EDIT_COM2_TCPPORT,     EditData_COM2TCPPort, 16);

  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM3_SELDOWN, ButtonData_COM3SelDown));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM3_SELUP,   ButtonData_COM3SelUp));
  COMNameEdit[2] = new SDLEdit_t(ID_EDIT_COM3_COMNAME,     EditData_COM3COMName, 16);
  TCPAddrEdit[2] = new SDLEdit_t(ID_EDIT_COM3_TCPADDR,     EditData_COM3TCPAddr, 16);
  TCPPortEdit[2] = new SDLEdit_t(ID_EDIT_COM3_TCPPORT,     EditData_COM3TCPPort, 16);

  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM4_SELDOWN, ButtonData_COM4SelDown));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_COM4_SELUP,   ButtonData_COM4SelUp));
  COMNameEdit[3] = new SDLEdit_t(ID_EDIT_COM4_COMNAME,     EditData_COM4COMName, 16);
  TCPAddrEdit[3] = new SDLEdit_t(ID_EDIT_COM4_TCPADDR,     EditData_COM4TCPAddr, 16);
  TCPPortEdit[3] = new SDLEdit_t(ID_EDIT_COM4_TCPPORT,     EditData_COM4TCPPort, 16);

  Controls.GetControl(ID_BUTTON_OK)->SetActive(true);
  Controls.GetControl(ID_BUTTON_CANCEL)->SetActive(true);

  // Set initial control states

//  for (Port = 0 ; Port < 4 ; Port++)
//  {
//    SERIAL_GetConfig(
//      Port,
//      Mapping[Port],
//      COMName[Port],
//      TCPAddress[Port],
//      TCPPort[Port]);
//
//    Controls.GetControl(ID_BUTTON_COM1_SELDOWN + Port*100)->SetActive(true);
//    Controls.GetControl(ID_BUTTON_COM1_SELUP + Port*100)->SetActive(true);
//
//    COMNameEdit[Port]->SetActive(true);
//    COMNameEdit[Port]->SetText(COMName[Port]);
//    TCPAddrEdit[Port]->SetActive(true);
//    TCPAddrEdit[Port]->SetText(TCPAddress[Port]);
//    TCPPortEdit[Port]->SetActive(true);
//    TCPPortEdit[Port]->SetText(TCPPort[Port]);
//
//    if (Mapping[Port] == SERIAL_TCP_SERVER)
//    {
//      Controls.AddControl(TCPPortEdit[Port]);
//    }
//    else if (Mapping[Port] == SERIAL_TCP_CLIENT)
//    {
//      Controls.AddControl(TCPAddrEdit[Port]);
//      Controls.AddControl(TCPPortEdit[Port]);
//    }
//    else if (Mapping[Port] == SERIAL_COM)
//    {
//      Controls.AddControl(COMNameEdit[Port]);
//    }
//  }

  // Create the window

  SDL_Window *DialogWindow =
    SDL_CreateWindow(
      "Configure serial ports",
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      320, 480,
      0);

  SDL_Surface *Surf = SDL_GetWindowSurface(DialogWindow);

  SDL_Event Event;
  int Done;
  int ControlId;
  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = 0;
  DRect.w = 320;
  DRect.h = 480;

  Done = 0;

  while (!Done)
  {
    // Redraw the dialog window

    SDL_FillRect(Surf, &DRect, 0x00C0C0C0);
    for (Port = 0 ; Port < 4 ; Port++)
    {
      sprintf(Buffer, "COM%d:", Port+1);
      SDLTEXT_DrawString(Surf, 14, 14 + Port * 104, 0x00000000, Buffer);

      GroupRect.x = 44;
      GroupRect.y = 34 + Port * 104;
      GroupRect.w = 232;
      GroupRect.h = 24;
      Draw3DRect(Surf, GroupRect, 0x00808080, 0x00FFFFFF);
      SDLTEXT_GetSize(SerMappingStr[Mapping[Port]], sx, sy, strlen(SerMappingStr[Mapping[Port]]));
      SDLTEXT_DrawString(Surf, 160-sx/2, 46 + Port * 104 - sy/2, 0x00000000, SerMappingStr[Mapping[Port]]);

      GroupRect.x = 10;
      GroupRect.y = 30 + Port * 104;
      GroupRect.w = 300;
      GroupRect.h = 80;
      Draw3DRect(Surf, GroupRect, 0x00808080, 0x00FFFFFF);

      if (Mapping[Port] == SERIAL_TCP_SERVER)
      {
        SDLTEXT_DrawString(Surf, 18, 88 + Port * 104, 0x00000000, "TCP Port:");
      }
      else if (Mapping[Port] == SERIAL_TCP_CLIENT)
      {
        SDLTEXT_DrawString(Surf, 18, 64 + Port * 104, 0x00000000, "TCP Addr:");
        SDLTEXT_DrawString(Surf, 18, 88 + Port * 104, 0x00000000, "TCP Port:");
      }
      else if (Mapping[Port] == SERIAL_COM)
      {
        SDLTEXT_DrawString(Surf, 18, 64 + Port * 104, 0x00000000, "Device:");
      }
    }

    Controls.Draw(Surf);
    SDL_UpdateWindowSurface(DialogWindow);

    SDL_WaitEvent(&Event);

    // Process the SDL event and determine the Control event

    if (IsEventForWindow(Event, SDL_GetWindowID(DialogWindow)))
    {
      ControlId = Controls.HandleEvent(Event);
      Port = (ControlId / 100) - 2;

      switch (ControlId)
      {
        case ID_BUTTON_COM1_SELDOWN:
        case ID_BUTTON_COM2_SELDOWN:
        case ID_BUTTON_COM3_SELDOWN:
        case ID_BUTTON_COM4_SELDOWN:
          if (Mapping[Port] == SERIAL_TCP_SERVER)
          {
            Controls.RemoveControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_TCP_CLIENT)
          {
            Controls.RemoveControl(TCPAddrEdit[Port]);
            Controls.RemoveControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_COM)
          {
            Controls.RemoveControl(COMNameEdit[Port]);
          }

          if (Mapping[Port] > SERIAL_UNUSED)
          {
            Mapping[Port] = (SerialMapping_t) (Mapping[Port] - 1);
          }
          else
          {
            Mapping[Port] = SERIAL_COM;
          }

          if (Mapping[Port] == SERIAL_TCP_SERVER)
          {
            Controls.AddControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_TCP_CLIENT)
          {
            Controls.AddControl(TCPAddrEdit[Port]);
            Controls.AddControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_COM)
          {
            Controls.AddControl(COMNameEdit[Port]);
          }
          break;

        case ID_BUTTON_COM1_SELUP:
        case ID_BUTTON_COM2_SELUP:
        case ID_BUTTON_COM3_SELUP:
        case ID_BUTTON_COM4_SELUP:
          if (Mapping[Port] == SERIAL_TCP_SERVER)
          {
            Controls.RemoveControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_TCP_CLIENT)
          {
            Controls.RemoveControl(TCPAddrEdit[Port]);
            Controls.RemoveControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_COM)
          {
            Controls.RemoveControl(COMNameEdit[Port]);
          }

          if (Mapping[Port] < SERIAL_COM)
          {
            Mapping[Port] = (SerialMapping_t) (Mapping[Port] + 1);
          }
          else
          {
            Mapping[Port] = SERIAL_UNUSED;
          }

          if (Mapping[Port] == SERIAL_TCP_SERVER)
          {
            Controls.AddControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_TCP_CLIENT)
          {
            Controls.AddControl(TCPAddrEdit[Port]);
            Controls.AddControl(TCPPortEdit[Port]);
          }
          else if (Mapping[Port] == SERIAL_COM)
          {
            Controls.AddControl(COMNameEdit[Port]);
          }
          break;

        case ID_BUTTON_OK:
          for (Port = 0 ; Port < 4 ; Port++)
          {
            char TmpCOMName[128];

            int TLen;
            TLen = 128;
            COMNameEdit[Port]->GetText(TmpCOMName, &TLen);
            TLen = 128;
            TCPAddrEdit[Port]->GetText(TCPAddress[Port], &TLen);
            TLen = 64;
            TCPPortEdit[Port]->GetText(TCPPort[Port], &TLen);

///            SERIAL_Configure(Port, Mapping[Port], TmpCOMName);
          }
          Done = 1;
          break;

        case ID_BUTTON_CANCEL:
          Done = 1;
          break;

        default:
          break;
      }
    }

  }


  // Release edit controls.
  // These may not be in the control list, and hence not automatically
  // deleted when the control list is destroyed.
  // Easiest solution is to remove all edit controls from the control list
  // then delete the edit control.
  for (Port = 0 ; Port < 4 ; Port++)
  {
    if (Mapping[Port] == SERIAL_TCP_SERVER)
    {
      Controls.RemoveControl(TCPPortEdit[Port]);
    }
    else if (Mapping[Port] == SERIAL_TCP_CLIENT)
    {
      Controls.RemoveControl(TCPAddrEdit[Port]);
      Controls.RemoveControl(TCPPortEdit[Port]);
    }
    else if (Mapping[Port] == SERIAL_COM)
    {
      Controls.RemoveControl(COMNameEdit[Port]);
    }

    delete COMNameEdit[Port];
    delete TCPAddrEdit[Port];
    delete TCPPortEdit[Port];
  }

  SDL_DestroyWindow(DialogWindow);
}
