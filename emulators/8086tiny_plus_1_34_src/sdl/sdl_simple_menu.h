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

#ifndef __SDL_SIMPLE_MENU_H
#define __SDL_SIMPLE_MENU_H

#include <SDL2/SDL.h>

#define SDLMENU_NULLID    0
#define SDLMENU_SEPARATOR 1

typedef enum
{
  SDLMENU_UNCHECKED,
  SDLMENU_CHECKED
} SDLMENU_CheckState;

struct SDLMENU_Item
{
  const char *Text;
  int  ItemId;
  SDLMENU_CheckState  Checked;
  struct SDLMENU_Item *Next;
};

struct SDLMENU_Menu
{
  const char *Text;
  struct SDLMENU_Menu *Next;
  struct SDLMENU_Item *ItemList;
  int    HitLeft;     /* Hit region. Filled out by menu handler. */
  int    HitTop;      /* These values are for internal use only. */
  int    HitWidth;
  int    HitHeight;
};

/* =============================================================================
 * FUNCTION: SDLMENU_SetMenu
 *
 * DESCRIPTION:
 * Set the menu to be dislayed in a window.
 *
 * PARAMETERS:
 *
 *   win      : The window to contain the menu
 *
 *   FontSpec : The X font specified string for the menu, or NULL for the
 *              default menu font (Helvetica 12).
 *
 *   Repaint  : A pointer to the function that performs the repainting of
 *              windows that may be obscured by the popup menus.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void SDLMENU_SetMenu(
  SDL_Window *win,
  struct SDLMENU_Menu *Menu,
  void (*Repaint)(void));

/* =============================================================================
 * FUNCTION: SDLMENU_SetCheck
 *
 * DESCRIPTION:
 * Set the checkmark state of a menu item.
 *
 * PARAMETERS:
 *
 *   ItemId     : The ItemId of the menu item.
 *
 *   CheckState : The new check state for the menu item.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void SDLMENU_SetCheck(int ItemId, SDLMENU_CheckState CheckState);

/* =============================================================================
 * FUNCTION: SDLMENU_GetMenuHeight
 *
 * DESCRIPTION:
 * Get the height of the currently managed menu bar in pixels.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   The menu bar height in pixels.
 */
int SDLMENU_GetMenuHeight(void);

/* =============================================================================
 * FUNCTION: SDLMENU_Redraw
 *
 * DESCRIPTION:
 * Redraw the menu bar.
 *
 * PARAMETERS:
 *
 *   None.
 *
 * RETURN VALUE:
 *
 *   None.
 */
void SDLMENU_Redraw(void);

/* =============================================================================
 * FUNCTION: SDLMENU_HandleEvent
 *
 * DESCRIPTION:
 * Perform event handling for the menu.
 * If the X event activates the menu then further X events will be processed
 * until an item is selected or the menu is deactivated.
 *
 * PARAMETERS:
 *
 *   Event : The X event to be handled.
 *
 * RETURN VALUE:
 *
 *   Returns The selected Menu Item or XMENU_NULLID if the event is handled
 *           by the menu.
 *   Returns -1 if the event is not handled by the menu.
 */
int SDLMENU_HandleEvent(SDL_Event &Event);


 #endif // __SDL_SIMPLE_MENU_H
