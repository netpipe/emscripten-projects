// SDL Simple Menu

#define MENU_EMULATION_RESET          101
#define MENU_EMULATION_QUIT           102

#define MENU_CONFIG_SET_FD_IMAGE      201
#define MENU_CONFIG_SET_HD_IMAGE      202
#define MENU_CONFIG_SET_BIOS_IMAGE    203
#define MENU_CONFIG_SET_SPEED         204
#define MENU_CONFIG_SCALE1X           205
#define MENU_CONFIG_SCALE2X           206
#define MENU_CONFIG_CGA_FONT          207
#define MENU_CONFIG_VGA_FONT          208
#define MENU_CONFIG_SERIAL            209
#define MENU_CONFIG_SOUND             210
#define MENU_CONFIG_SAVE_DEFAULT      211
#define MENU_CONFIG_SAVE              212
#define MENU_CONFIG_LOAD              213

#define MENU_DEBUG_START              301

/* debug menu definitions */

struct SDLMENU_Item Debug_Start_Item =
{
  "Start Debugger...",  MENU_DEBUG_START, SDLMENU_UNCHECKED, NULL
};

struct SDLMENU_Menu Debug_Menu =
{
  "Debug",
  NULL,
  &Debug_Start_Item,
  0, 0, 0, 0
};

/* Configuration menu definitions */

struct SDLMENU_Item Config_Load_Item = /* unused */
{
  "Load Configuration...",  MENU_CONFIG_LOAD, SDLMENU_UNCHECKED, NULL
};

struct SDLMENU_Item Config_Save_Item =
{
  "Save Configuration...",  MENU_CONFIG_SAVE, SDLMENU_UNCHECKED, &Config_Load_Item
};

struct SDLMENU_Item Config_Save_Default_Item =
{
  "Save Default Configuration",  MENU_CONFIG_SAVE_DEFAULT, SDLMENU_UNCHECKED, &Config_Save_Item
};

struct SDLMENU_Item Config_Separator4_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_Save_Default_Item
};

struct SDLMENU_Item Config_UseVGAFont_Item =
{
  "Use VGA Font",  MENU_CONFIG_VGA_FONT, SDLMENU_UNCHECKED, &Config_Separator4_Item
};

struct SDLMENU_Item Config_UseCGAFont_Item =
{
  "Use CGA Font",  MENU_CONFIG_CGA_FONT, SDLMENU_UNCHECKED, &Config_UseVGAFont_Item
};

struct SDLMENU_Item Config_Separator3_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_UseCGAFont_Item
};

struct SDLMENU_Item Config_Scale2X_Item =
{
  "Scale x2",  MENU_CONFIG_SCALE2X, SDLMENU_UNCHECKED, &Config_Separator3_Item
};

struct SDLMENU_Item Config_Scale1X_Item =
{
  "Scale x1",  MENU_CONFIG_SCALE1X, SDLMENU_UNCHECKED, &Config_Scale2X_Item
};

struct SDLMENU_Item Config_Separator2_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_Scale1X_Item
};

struct SDLMENU_Item Config_Sound_Item =
{
  "Configure sound...",  MENU_CONFIG_SOUND, SDLMENU_UNCHECKED, &Config_Separator2_Item
};

struct SDLMENU_Item Config_Serial_Item =
{
  "Set Serial Ports...",  MENU_CONFIG_SERIAL, SDLMENU_UNCHECKED, &Config_Sound_Item
};

struct SDLMENU_Item Config_Speed_Item =
{
  "CPU Speed...",  MENU_CONFIG_SET_SPEED, SDLMENU_UNCHECKED, &Config_Serial_Item
};

struct SDLMENU_Item Config_Separator1_Item =
{
  NULL,  SDLMENU_SEPARATOR, SDLMENU_UNCHECKED, &Config_Speed_Item
};

struct SDLMENU_Item Config_BIOSImage_Item =
{
  "Set BIOS Image...",  MENU_CONFIG_SET_BIOS_IMAGE, SDLMENU_UNCHECKED, &Config_Separator1_Item
};

struct SDLMENU_Item Config_HDImage_Item =
{
  "Set HD Image...",  MENU_CONFIG_SET_HD_IMAGE, SDLMENU_UNCHECKED, &Config_BIOSImage_Item
};

struct SDLMENU_Item Config_FDImage_Item =
{
  "Set FD Image...",  MENU_CONFIG_SET_FD_IMAGE, SDLMENU_UNCHECKED, &Config_HDImage_Item
};


struct SDLMENU_Menu Config_Menu =
{
  "Configuration",
  &Debug_Menu,
  &Config_FDImage_Item,
  0, 0, 0, 0
};


/* Emulation menu definitions */

struct SDLMENU_Item Emulation_Quit_Item =
{
  "Quit",  MENU_EMULATION_QUIT, SDLMENU_UNCHECKED, NULL
};

struct SDLMENU_Item Emulation_Reset_Item =
{
  "Reset", MENU_EMULATION_RESET, SDLMENU_UNCHECKED, &Emulation_Quit_Item
};

struct SDLMENU_Menu Emulation_Menu =
{
  "Emulation",
  &Config_Menu,
  &Emulation_Reset_Item,
  0, 0, 0, 0
};
