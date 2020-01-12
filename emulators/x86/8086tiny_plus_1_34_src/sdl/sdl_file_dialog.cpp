// =============================================================================
// File: sdl_file_dialog.cpp
//
// Description:
// File request dialog SDL implementation.
//
// This work is licensed under the MIT License. See included LICENSE.TXT.
//

#include <stdio.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>

#include "file_dialog.h"

#include "sdl_simple_text.h"
#include "sdl_control_list.h"
#include "sdl_button.h"
#include "sdl_checkbox.h"
#include "sdl_edit.h"
#include "sdl_listbox.h"

#define ID_BUTTON_CANCEL     101
#define ID_BUTTON_OK         102

#define ID_EDIT_FILENAME 201
#define ID_LISTBOX_FILES 202
#define ID_LISTBOX_DIRS  203

#ifdef _WIN32
#define DIR_SEPARATOR "\\"
#else
#define DIR_SEPARTOR "/"
#endif // _WIN32

#include <unistd.h>
// =============================================================================
// Local Variables
//

// Last directory used by the file open dialog

static bool FileDialogStartSet = false;
static char FileDialogStartDir[1024];

// Control Data

static SDLControlData_t ButtonData_Cancel =
{
  { 10, 450, 80, 24 },
  "Cancel",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ButtonData_OK =
{
  { 550, 450, 80, 24 },
  "OK",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t EditData_Filename =
{
  { 90, 36, 540, 24 },
  "Filename",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00C0C0C0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ListboxData_FileList =
{
  { 10, 80, 390, 340 },
  "Filelist",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00A0A0E0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

static SDLControlData_t ListboxData_DirList =
{
  { 410, 80, 220, 340 },
  "Dirlist",
  {
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00E0E0E0 }, // SDLCONTROL_STATE_INACTIVE,
    { 0x00C0C0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_NORMAL,
    { 0x00C0E0C0, 0x00FFFFFF, 0x00A0A0A0, 0x00000000 }, // SDLCONTROL_STATE_ACTIVE_HILIGHT,
    { 0x00A0A0E0, 0x00A0A0A0, 0x00FFFFFF, 0x00000000 }  // SDLCONTROL_STATE_ACTIVE_PRESSED,
  }
};

// =============================================================================
// Local Functions
//

static void SetDirLists(SDLListBox_t *FileList, SDLListBox_t *DirList)
{
  DIR *Dirent_Dir = NULL;

  FileList->Clear();
  DirList->Clear();

  Dirent_Dir = opendir(".");

  struct dirent *entry;
  entry = readdir(Dirent_Dir);
  while  (entry != NULL)
  {
    struct stat s;

    if (stat(entry->d_name, &s) == -1)
    {
      printf("stat failed for '%s'\n", entry->d_name);
    }

    if ((s.st_mode & S_IFMT) == S_IFDIR)
    {
      DirList->AddLine(entry->d_name);
    }
    else
    {
      FileList->AddLine(entry->d_name);
    }

    entry = readdir(Dirent_Dir);
  }

  closedir(Dirent_Dir);
}

bool DoFileDialog(
  const char *Title,
  char *fname, int filelen,
  const char *filter,
  bool OpenExisting)
{
  SDLControlList_t Controls;
  SDLEdit_t    *FilenameEdit;
  SDLListBox_t *FileList;
  SDLListBox_t *DirList;
  bool FileEditSet = false;
  bool GotFilename = false;
  char idir[1024];
  char cdir[1024];
  char tname[1024];
  int tlen;

  // Create the dialog controls

  FilenameEdit = new SDLEdit_t(ID_EDIT_FILENAME, EditData_Filename, 64);
  FilenameEdit->SetText("");

  FileList = new SDLListBox_t(ID_LISTBOX_FILES, ListboxData_FileList);
  DirList  = new SDLListBox_t(ID_LISTBOX_DIRS, ListboxData_DirList);

  Controls.AddControl(new SDLButton_t(ID_BUTTON_OK, ButtonData_OK));
  Controls.AddControl(new SDLButton_t(ID_BUTTON_CANCEL, ButtonData_Cancel));
  Controls.AddControl(FilenameEdit);
  Controls.AddControl(FileList);
  Controls.AddControl(DirList);

  Controls.GetControl(ID_BUTTON_OK)->SetActive(true);
  Controls.GetControl(ID_BUTTON_CANCEL)->SetActive(true);
  if (!OpenExisting)
  {
    FilenameEdit->SetActive(true);
  }
  FileList->SetActive(true);
  DirList->SetActive(true);

  // Create the window

  SDL_Window *DialogWindow =
    SDL_CreateWindow(
      Title,
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      640, 480,
      0);

  SDL_Surface *Surf = SDL_GetWindowSurface(DialogWindow);

  SDL_Event Event;
  int Done;
  int ControlId;
  SDL_Rect DRect;
  DRect.x = 0;
  DRect.y = 0;
  DRect.w = 640;
  DRect.h = 480;

  Done = 0;

  getcwd(idir, 1024);

  if (FileDialogStartSet)
  {
    chdir(FileDialogStartDir);
  }

  getcwd(cdir, 1024);

  SetDirLists(FileList, DirList);

  while (!Done)
  {
    // Redraw the dialog window

    SDL_FillRect(Surf, &DRect, 0x00C0C0C0);
    SDLTEXT_DrawString(Surf, 10, 14, 0x00000000, "Path:");
    SDLTEXT_DrawString(Surf, 60, 14, 0x00000000, cdir);
    SDLTEXT_DrawString(Surf, 10, 40, 0x00000000, "Filename:");
    SDLTEXT_DrawString(Surf, 10, 64, 0x00000000, "Files:");
    SDLTEXT_DrawString(Surf, 410, 64, 0x00000000, "Directories:");
    Controls.Draw(Surf);
    SDL_UpdateWindowSurface(DialogWindow);

    SDL_WaitEvent(&Event);

    // Process the SDL event and determine the Control event

    if (IsEventForWindow(Event, SDL_GetWindowID(DialogWindow)))
    {
      ControlId = Controls.HandleEvent(Event);

      switch (ControlId)
      {
        case ID_BUTTON_OK:
          Done = 1;
          tlen = 1024;
          FilenameEdit->GetText(tname, &tlen);

          if (OpenExisting && FileEditSet)
          {
          //  snprintf(fname, filelen, "%s%s%s", cdir, DIR_SEPARATOR, tname);
            GotFilename = true;
          }
          else if (!OpenExisting && (tlen > 0))
          {
//            snprintf(fname, filelen, "%s%s%s", cdir, DIR_SEPARATOR, tname);
            GotFilename = true;
          }

          getcwd(FileDialogStartDir, 1024);
          FileDialogStartSet = true;
          break;

        case ID_BUTTON_CANCEL:
          Done = 1;

          getcwd(FileDialogStartDir, 1024);
          FileDialogStartSet = true;
          break;

        case ID_LISTBOX_FILES:
        {
          int Idx = FileList->GetSelIdx();
          char *FileName = FileList->GetText(Idx);
          if (FileName != NULL)
          {
            FilenameEdit->SetText(FileName);
            FileEditSet = true;
          }
          break;
        }

        case ID_LISTBOX_DIRS:
          if (DirList->IsDoubleClickEvent())
          {
            int Idx = DirList->GetSelIdx();
            char *DirName = DirList->GetText(Idx);
            if (DirName != NULL)
            {
              chdir(DirName);
              SetDirLists(FileList, DirList);

              getcwd(cdir, 1024);

              FilenameEdit->SetText("");
              FileEditSet = false;
            }
          }
          break;

        default:
          break;
      }
    }

  }

  SDL_DestroyWindow(DialogWindow);

  // Change back to the working directory.
  chdir(idir);

  return GotFilename;
}


// =============================================================================
// Exported Functions
//

bool OpenFileDialog(const char *Title, char *fname, int filelen, const char *filter)
{
  return DoFileDialog(Title, fname, filelen, filter, true);
}

bool SaveFileDialog(const char *Title, char *fname, int filelen, const char *filter)
{
  return DoFileDialog(Title, fname, filelen, filter, false);
}
