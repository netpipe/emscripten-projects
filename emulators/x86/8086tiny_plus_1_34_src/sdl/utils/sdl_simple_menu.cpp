/* =============================================================================
 * PROGRAM:  CODE LIBRARY
 * FILENAME: sdl_simple_menu.h
 *
 * DESCRIPTION:
 * This module provides a simple menu bar capability for SDL programs.
 * It can currently only handle a single window/menu at a time.
 * To use this library in an application:
 *   1. Create the widow to contain the menu.
 *   2. Call SDLMENU_SetMenu to set the menu to be displayed in that window
 *      This must provide a pointer to a function to repaint the windows
 *      that may be obscured by the menu.
 *   3. Call SDLMENU_GetMenuHeight to get the height of the menu bar.
 *      The application should be careful not to draw in this area.
 *   4. When processing an SDL event the main event loop should call
 *      SDLMENU_HandleEvent.
 *      This function will return either:
 *      . -1 if the menu didn't handle this   event, in which case the
 *        application needs to handle it, or
 *      . The selected menu item id if the event was handled by the menu.
 *        In this case the application shouldn't process this event, but should
 *        act on the returned menu selection id.
 *   6. Call SDLMENU_Redraw whenever the window containing the menu gets an
 *      expose event.
 *
 * =============================================================================
 * COPYRIGHT:
 *
 * Copyright (c) 2003, Julian Olds
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
 * None.
 *
 * =============================================================================
 * EXPORTED FUNCTIONS
 *
 * SDLMENU_SetMenu       - Set the menu and menu display window
 * SDLMENU_SetCheck      - Set the checkmark state for a menu item
 * SDLMENU_GetMenuHeight - Get the menu height in pixels
 * SDLMENU_Redraw        - Redraw the menu
 * SDLMENU_HandleEvent   - Process input events for the menu selection
 *
 * =============================================================================
 */

#include "sdl_simple_menu.h"
#include "sdl_simple_text.h"

/* =============================================================================
 * Local variables
 */

static SDL_Window  *MenuWindow;

static struct SDLMENU_Menu *The_Menu;
static void (*RepaintWindow)(void);

/* Current popup menu size */

static int PopupLeft;
static int PopupTop;
static int PopupWidth;
static int PopupHeight;

static int ItemHeight;

static SDLMENU_Menu *MouseOverMenu = NULL;

static const unsigned char CheckString[2] = { 251, 0 };

/* =============================================================================
 * Local functions
 */


/* =============================================================================
 * FUNCTION: SDLMENU_ItemHit
 *
 * DESCRIPTION:
 * Determine which menu item in the current popup menu is at location x, y
 *
 * PARAMETERS:
 *
 *   Menu : The currently active popup menu
 *
 *   x    : The x coordinate to test.
 *
 *   y    : The y coordinate to test.
 *
 * RETURN VALUE:
 *
 *   The ItemId of the menu item at (x, y) or XMENU_NUMMID if none.
 */
static int SDLMENU_ItemHit(struct SDLMENU_Menu *Menu, int x, int y)
{
  struct SDLMENU_Item *Item;
  int ItemNo;

  if ((x < PopupLeft) || (x > (PopupLeft + PopupWidth)) ||
      (y < PopupTop) || (y > (PopupTop + PopupHeight)))
  {
    return SDLMENU_NULLID;
  }

  ItemNo = (y - PopupTop) / ItemHeight;

  Item = Menu->ItemList;

  while ((Item != NULL) && (ItemNo > 0))
  {
    Item = Item->Next;
    ItemNo--;
  }

  if ((Item != NULL) && (Item->ItemId != SDLMENU_SEPARATOR))
  {
    return Item->ItemId;
  }
  else
  {
    return SDLMENU_NULLID;
  }
}

/* =============================================================================
 * FUNCTION: SDLMENU_MenuHit
 *
 * DESCRIPTION:
 * Identify which menu is at location x, y.
 *
 * PARAMETERS:
 *
 *   Menu : A pointer to the menu structure
 *
 *   x    : The x coordinate to test.
 *
 *   y    : The y coordinate to test.
 *
 * RETURN VALUE:
 *
 *   A pointer to the menu on the menu bar at (x, y) or NULL if none.
 */
static struct SDLMENU_Menu *SDLMENU_MenuHit(struct SDLMENU_Menu *Menu, int x, int y)
{
  struct SDLMENU_Menu *Pos;
  int FoundHit;

  Pos = Menu;
  FoundHit = 0;

  while ((Pos != NULL) && !FoundHit)
  {
    if ((x >= Pos->HitLeft) && (x < Pos->HitLeft + Pos->HitWidth) &&
        (y >= Pos->HitTop) && (y < Pos->HitTop + Pos->HitHeight))
    {
      FoundHit = 1;
    }
    else
    {
      Pos = Pos->Next;
    }
  }

  return Pos;
}

/* =============================================================================
 * FUNCTION: SDLMENU_PaintPopup
 *
 * DESCRIPTION:
 * Paints the popup menu window with the items of a menu.
 *
 * PARAMETERS:
 *
 *   Popup : The popup window to be painted.
 *
 *   Menu  : The menu for the popup window.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void SDLMENU_PaintPopup(struct SDLMENU_Menu *Menu, int mx, int my)
{
  struct SDLMENU_Item *Item;
  int Top;

  int ItemOver = SDLMENU_ItemHit(Menu, mx, my);
  int ItemOverIdx;

  SDL_Surface *Surf = SDL_GetWindowSurface(MenuWindow);

  SDL_Rect PopupRect;
  PopupRect.x = PopupLeft;
  PopupRect.y = PopupTop;
  PopupRect.w = PopupWidth;
  PopupRect.h = PopupHeight;

  SDL_FillRect(Surf, &PopupRect, 0x808080);

  PopupRect.x += 1;
  PopupRect.y += 1;
  PopupRect.w -= 2;
  PopupRect.h -= 2;
  SDL_FillRect(Surf, &PopupRect, 0xC0C0C0);

  if (ItemOver != SDLMENU_NULLID)
  {
    ItemOverIdx = 0;
    Item = Menu->ItemList;
    while (Item->ItemId != ItemOver)
    {
      Item = Item->Next;
      ItemOverIdx++;
    }

    PopupRect.y = PopupTop + ItemOverIdx * ItemHeight;
    PopupRect.h = ItemHeight;
    SDL_FillRect(Surf, &PopupRect, 0xFFF0D0);
  }

  Top = PopupTop+2;
  Item = Menu->ItemList;

  while (Item != NULL)
  {
    if (Item->ItemId == SDLMENU_SEPARATOR)
    {
      PopupRect.x = PopupLeft+4;
      PopupRect.y = Top+9;
      PopupRect.w = PopupWidth-8;
      PopupRect.h = 1;

      SDL_FillRect(Surf, &PopupRect, 0x808080);

      PopupRect.y = Top+10;

      SDL_FillRect(Surf, &PopupRect, 0xE0E0E0);
   }

    Top += ItemHeight;

    Item = Item->Next;
  }

  Top = PopupTop+2;
  Item = Menu->ItemList;

  while (Item != NULL)
  {
    if (Item->ItemId != SDLMENU_SEPARATOR)
    {
      if (Item->Checked)
      {
        SDLTEXT_DrawString(Surf, PopupLeft + 8, Top, 0x00000000, (const char *) CheckString);
      }

      SDLTEXT_DrawString(Surf, PopupLeft + 24, Top, 0x00000000, Item->Text);
    }

    Top += ItemHeight;

    Item = Item->Next;
  }

  SDL_UpdateWindowSurface(MenuWindow);
}

/* =============================================================================
 * FUNCTION: SDLMENU_ActivatePopup
 *
 * DESCRIPTION:
 * Create the popup menu window.
 *
 * PARAMETERS:
 *
 *   Menu : The popup menu to activate.
 *
 * RETURN VALUE:
 *
 *   None.
 */
static void SDLMENU_ActivatePopup(struct SDLMENU_Menu *Menu)
{
  struct SDLMENU_Item *Item;
  int ItemWidth;

  int WindowW, WindowH;

  SDL_GetWindowSize(MenuWindow, &WindowW, &WindowH);

  /* Calculate the popup menu size */
  PopupWidth = 0;
  PopupHeight = 0;

  Item = Menu->ItemList;
  while (Item != NULL)
  {
    if (Item->ItemId != SDLMENU_SEPARATOR)
    {
      ItemWidth = strlen(Item->Text) * 8 + 32;
      if (ItemWidth > PopupWidth) PopupWidth = ItemWidth;
    }
    PopupHeight += ItemHeight;

    Item = Item->Next;
  }

  /* Work out the popup menu position in the parent window */
  if ((Menu->HitLeft + PopupWidth) > WindowW)
  {
    PopupLeft = WindowW - PopupWidth;
  }
  else
  {
    PopupLeft = Menu->HitLeft;
  }

  PopupTop = Menu->HitTop + Menu->HitHeight + 1;
}

/* =============================================================================
 * FUNCTION: SDLMENU_HandleSelection
 *
 * DESCRIPTION:
 * Handle the selection from the menu.
 * This activates the initially selected menu and processes all x events until
 * either a menu item is selected or the menu is deactivated.
 *
 * PARAMETERS:
 *
 *   Menu : A pointer to the menu structure for the menus.
 *
 *   x    : The x coordinate of the initial selection
 *
 *   y    : The y coordinate of the initial selection
 *
 * RETURN VALUE:
 *
 *   The ItemId of the menu item selected or XMENU_NULLID if none.
 */
static int SDLMENU_HandleSelection(struct SDLMENU_Menu *Menu,  int x, int y)
{
  struct SDLMENU_Menu *Current;
  struct SDLMENU_Menu *NewMenu;
  SDL_Event Event;
  int Done;
  int SelectId = SDLMENU_NULLID;

  /* Activate the Popup menu for the initially selected menu */
  Current = SDLMENU_MenuHit(The_Menu, x, y);

  if (Current != NULL)
  {
    SDLMENU_ActivatePopup(Current);
  }
  else
  {
    return SelectId;
  }

  SDLMENU_PaintPopup(Current, x, y);

  /* Prepare for event handling */
  Done = 0;

  /*
   * Handle events until either a menu item is selected of the menu is
   * Deactivated.
   */

  while (!Done)
  {
    SDL_WaitEvent(&Event);

    switch (Event.type)
    {
      case SDL_MOUSEBUTTONDOWN:
      {
        switch (Event.button.button)
        {
          case SDL_BUTTON_LEFT:
            x = Event.button.x;
            y = Event.button.y;

            SelectId = SDLMENU_ItemHit(Current, x, y);
            Done = 1;
            break;

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

            SelectId = SDLMENU_ItemHit(Current, x, y);
            if (SelectId != SDLMENU_NULLID)
            {
              Done = 1;
            }
            else
            {
              NewMenu = SDLMENU_MenuHit(The_Menu, x, y);
              if (NewMenu == NULL)
              {
                Done = 1;
              }
              else
              {
                if (NewMenu != Current)
                {
                  Current = NewMenu;
                  MouseOverMenu = Current;
                  if (RepaintWindow != NULL) RepaintWindow();
                  SDLMENU_Redraw();
                  SDLMENU_ActivatePopup(Current);
                  SDLMENU_PaintPopup(Current, x, y);
                }
              }
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

        NewMenu = SDLMENU_MenuHit(The_Menu, x, y);
        if (NewMenu != NULL)
        {
          if (NewMenu != Current)
          {
            Current = NewMenu;
            MouseOverMenu = Current;
            if (RepaintWindow != NULL) RepaintWindow();
            SDLMENU_Redraw();
            SDLMENU_ActivatePopup(Current);
          }
        }
        SDLMENU_PaintPopup(Current, x, y);
        break;
      }
    }
  }

  MouseOverMenu = NULL;

  return SelectId;
}

/* =============================================================================
 * Exported functions
 */

/* =============================================================================
 * FUNCTION: SDLMENU_SetMenu
 */
void SDLMENU_SetMenu(SDL_Window *win,
                     struct SDLMENU_Menu *Menu,
                     void (*Repaint)(void))
{
  MenuWindow = win;

  The_Menu = Menu;
  RepaintWindow = Repaint;

  ItemHeight = 20;

  /* Draw the menu */

  SDLMENU_Redraw();
}

/* =============================================================================
 * FUNCTION: SDLMENU_SetCheck
 */
void SDLMENU_SetCheck(int ItemId, SDLMENU_CheckState CheckState)
{
  struct SDLMENU_Menu *Menu;
  struct SDLMENU_Item *Item;
  int Found;

  Found = 0;
  Menu = The_Menu;

  while ((Menu != NULL) && !Found)
  {
    Item = Menu->ItemList;

    while ((Item != NULL) && !Found)
    {
      if (Item->ItemId == ItemId)
      {
        Item->Checked = CheckState;
        Found = 1;
      }

      Item = Item->Next;
    }

    Menu = Menu->Next;
  }
}

/* =============================================================================
 * FUNCTION: SDLMENU_GetMenuHeight
 */
int SDLMENU_GetMenuHeight(void)
{
  return 26;
}

/* =============================================================================
 * FUNCTION: SDLMENU_Redraw
 */
void SDLMENU_Redraw(void)
{
  SDL_Surface *Surf = SDL_GetWindowSurface(MenuWindow);
  int WindowW, WindowH;
  int MenuWidth;
  int MenuHeight;
  int MenuLeft;
  int MenuTop;
  int StrWidth;
  SDLMENU_Menu *Pos;

  SDL_GetWindowSize(MenuWindow, &WindowW, &WindowH);

  MenuWidth = WindowW;
  MenuHeight = 26;

  SDL_Rect MenuRect;
  MenuRect.x = 0;
  MenuRect.y = 0;
  MenuRect.w = MenuWidth;
  MenuRect.h = MenuHeight-2;
  SDL_FillRect(Surf, &MenuRect, 0x00C0C0C0);

  MenuRect.y = MenuHeight-2;
  MenuRect.h = 2;
  SDL_FillRect(Surf, &MenuRect, 0x00808080);

  if (MouseOverMenu != NULL)
  {
    MenuRect.x = MouseOverMenu->HitLeft;
    MenuRect.y = MouseOverMenu->HitTop;
    MenuRect.w = MouseOverMenu->HitWidth;
    MenuRect.h = MouseOverMenu->HitHeight;
    SDL_FillRect(Surf, &MenuRect, 0x009F9070);

    MenuRect.x += 1;
    MenuRect.y += 1;
    MenuRect.w -= 2;
    MenuRect.h -= 2;
    SDL_FillRect(Surf, &MenuRect, 0x00FFF0D0);
  }

  Pos = The_Menu;
  MenuLeft = 8;
  MenuTop = 0;

  while (Pos != NULL)
  {
    StrWidth = SDLTEXT_DrawString(Surf, MenuLeft, MenuTop+4, 0x00000000, Pos->Text);

    Pos->HitLeft = MenuLeft-4;
    Pos->HitTop = MenuTop;

    Pos->HitWidth = StrWidth+8;
    Pos->HitHeight = 24;

    MenuLeft += StrWidth + 16;
    Pos = Pos->Next;
  }

}

/* =============================================================================
 * FUNCTION: SDLMENU_HandleEvent
 */
int SDLMENU_HandleEvent(SDL_Event &Event)
{
  int Selected;
  int x, y;

  Selected = SDLMENU_NULLID;

  switch (Event.type)
  {
    case SDL_MOUSEBUTTONDOWN:
    {
      switch (Event.button.button)
      {
        case SDL_BUTTON_LEFT:
          x = Event.button.x;
          y = Event.button.y;

          MouseOverMenu = SDLMENU_MenuHit(The_Menu, x, y);
          if (MouseOverMenu != NULL)
          {
            SDLMENU_Redraw();
            Selected = SDLMENU_HandleSelection(The_Menu, x, y);
          }

          return Selected;

        default:
          break;
      }
      break;
    }

    case SDL_MOUSEMOTION:
    {
      x = Event.motion.x;
      y = Event.motion.y;

      MouseOverMenu = SDLMENU_MenuHit(The_Menu, x, y);
      break;
    }

    default:
      break;
  }

  return -1;
}

