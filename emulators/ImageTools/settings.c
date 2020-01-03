/******************************* LICENCE **************************************
* Any code in this file may be redistributed or modified under the terms of
* the GNU General Public Licence as published by the Free Software 
* Foundation; version 2 of the licence.
****************************** END LICENCE ***********************************/

/******************************************************************************
* Author:
* Andrew Smith, http://littlesvr.ca/misc/contactandrew.php
*
* Contributors:
* 
******************************************************************************/

#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <gtk/gtk.h>
#include <dirent.h>
#include <libintl.h>
#include <time.h>

#include "isomaster.h"

/* used by iniparser */
dictionary* GBLsettingsDictionary;
/* home directory with full path or "/" if don't know it */
char* GBLuserHomeDir;
/* iso master runtime and stored settings */
AppSettings GBLappSettings;

extern GtkWidget* GBLmainWindow;
extern GtkWidget* GBLbrowserPaned;
extern char* GBLfsCurrentDir;
extern bool GBLisoPaneActive;
extern VolInfo GBLvolInfo;
extern bool GBLisoChangesProbable;
extern GtkListStore* GBLfsListStore;
extern GtkListStore* GBLisoListStore;
extern GtkWidget* GBLrecentlyOpenWidgets[5];

void buildImagePropertiesWindow(GtkWidget *widget, GdkEvent *event)
{
    GtkWidget* dialog;
    GtkWidget* table;
    GtkWidget* label;
    GtkWidget* field;
    GtkWidget* publisherField;
    GtkWidget* volNameField;
    GtkWidget* hBox;
    GtkWidget* rockridgeCheck;
    GtkWidget* jolietCheck;
    static const int fieldLen = 30; /* to display not length of contents */
    time_t timeT;
    char* timeStr;
    gint rc;
    
    /* do nothing if no image open */
    if(!GBLisoPaneActive)
        return;
    
    dialog = gtk_dialog_new_with_buttons(_("Image Information"),
                                         GTK_WINDOW(GBLmainWindow),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    g_signal_connect(dialog, "close", G_CALLBACK(rejectDialogCbk), NULL);
    
    table = gtk_table_new(1, 2, FALSE);
    gtk_table_set_row_spacings(GTK_TABLE(table), 5);
    gtk_table_set_col_spacings(GTK_TABLE(table), 5);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table, TRUE, TRUE, 0);
    gtk_widget_show(table);
    
    label = gtk_label_new(_("Creation time:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
    gtk_widget_show(label);
    
    timeT = bk_get_creation_time(&GBLvolInfo);
    timeStr = ctime(&timeT);
    timeStr[strlen(timeStr) - 1] = '\0'; /* remove the trailing newline */
    field = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(field), timeStr);
    //gtk_entry_set_editable(GTK_ENTRY(field), FALSE);
    gtk_widget_set_sensitive(field, FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(field), fieldLen);
    gtk_table_attach_defaults(GTK_TABLE(table), field, 1, 2, 0, 1);
    gtk_widget_show(field);
    
    label = gtk_label_new(_("Volume name:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);
    gtk_widget_show(label);
    
    volNameField = gtk_entry_new_with_max_length(32);
    gtk_entry_set_text(GTK_ENTRY(volNameField), bk_get_volume_name(&GBLvolInfo));
    gtk_entry_set_width_chars(GTK_ENTRY(volNameField), fieldLen);
    g_signal_connect(volNameField, "activate", (GCallback)acceptDialogCbk, dialog);
    gtk_table_attach_defaults(GTK_TABLE(table), volNameField, 1, 2, 1, 2);
    gtk_widget_show(volNameField);
    
    label = gtk_label_new(_("Publisher:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
    gtk_widget_show(label);
    
    publisherField = gtk_entry_new_with_max_length(128);
    gtk_entry_set_text(GTK_ENTRY(publisherField), bk_get_publisher(&GBLvolInfo));
    gtk_entry_set_width_chars(GTK_ENTRY(publisherField), fieldLen);
    g_signal_connect(publisherField, "activate", (GCallback)acceptDialogCbk, dialog);
    gtk_table_attach_defaults(GTK_TABLE(table), publisherField, 1, 2, 2, 3);
    gtk_widget_show(publisherField);
    
    hBox = gtk_hbox_new(FALSE, 5);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hBox, TRUE, TRUE, 5);
    gtk_widget_show(hBox);
    
    label = gtk_label_new(_("Filename types (both recommended):"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_box_pack_start(GTK_BOX(hBox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    rockridgeCheck = gtk_check_button_new_with_label("RockRidge");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rockridgeCheck), 
                                 GBLappSettings.filenameTypesToWrite & FNTYPE_ROCKRIDGE);
    gtk_box_pack_start(GTK_BOX(hBox), rockridgeCheck, TRUE, TRUE, 0);
    gtk_widget_show(rockridgeCheck);
    
    jolietCheck = gtk_check_button_new_with_label("Joliet");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(jolietCheck), 
                                 GBLappSettings.filenameTypesToWrite & FNTYPE_JOLIET);
    gtk_box_pack_start(GTK_BOX(hBox), jolietCheck, TRUE, TRUE, 0);
    gtk_widget_show(jolietCheck);
    
    gtk_widget_show(dialog);
    
    rc = gtk_dialog_run(GTK_DIALOG(dialog));
    if(rc == GTK_RESPONSE_ACCEPT)
    {
        const char* publisher;
        const char* volName;
        int rc2;
        GtkWidget* warningDialog;
        
        publisher = gtk_entry_get_text(GTK_ENTRY(publisherField));
        rc2 = bk_set_publisher(&GBLvolInfo, publisher);
        if(rc2 <= 0)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Invalid publisher '%s': '%s'"),
                                                   publisher,
                                                   bk_get_error_string(rc2));
            gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        
        volName = gtk_entry_get_text(GTK_ENTRY(volNameField));
        rc2 = bk_set_vol_name(&GBLvolInfo, volName);
        if(rc2 <= 0)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Invalid volume name '%s': '%s'"),
                                                   publisher,
                                                   bk_get_error_string(rc2));
            gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rockridgeCheck)))
            GBLappSettings.filenameTypesToWrite |= FNTYPE_ROCKRIDGE;
        else
            GBLappSettings.filenameTypesToWrite &= ~FNTYPE_ROCKRIDGE;
        
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(jolietCheck)))
            GBLappSettings.filenameTypesToWrite |= FNTYPE_JOLIET;
        else
            GBLappSettings.filenameTypesToWrite &= ~FNTYPE_JOLIET;
        
        GBLisoChangesProbable = true;
    }
    
    gtk_widget_destroy(dialog);
}

void findHomeDir(void)
{
    char* userHomeDir;
    int pathLen;
    
    userHomeDir = getenv("HOME");
    if(userHomeDir == NULL)
    /* pretend user's home is root */
    {
        printWarning("failed to getenv(\"HOME\"), using \"/\" as "
                     "home directory");
        GBLuserHomeDir = (char*)malloc(2);
        if(GBLuserHomeDir == NULL)
            fatalError("findHomeDir(): malloc(2) failed");
        strcpy(GBLuserHomeDir, "/");
        return;
    }
    
    /* MAKE sure userHomeDir is a valid directory */
    DIR* openDirTest;
    
    openDirTest = opendir(userHomeDir);
    if(openDirTest == NULL)
    {
        printf("failed to open directory described by $HOME: '%s'\n", 
                                                            userHomeDir);
        GBLuserHomeDir = (char*)malloc(2);
        if(GBLuserHomeDir == NULL)
            fatalError("findHomeDir(): malloc(2) failed");
        strcpy(GBLuserHomeDir, "/");
        return;
    }
    
    closedir(openDirTest);
    /* END MAKE sure userHomeDir is a valid directory */
    
    /* need the directory ending with a / (bkisofs rule) */
    pathLen = strlen(userHomeDir);
    if(userHomeDir[pathLen] == '/')
    {
        GBLuserHomeDir = (char*)malloc(pathLen + 1);
        if(GBLuserHomeDir == NULL)
            fatalError("findHomeDir(): malloc(pathLen + 1) failed");
        strcpy(GBLuserHomeDir, userHomeDir);
    }
    else
    {
        GBLuserHomeDir = (char*)malloc(pathLen + 2);
        if(GBLuserHomeDir == NULL)
            fatalError("findHomeDir(): malloc(pathLen + 2) failed");
        strcpy(GBLuserHomeDir, userHomeDir);
        strcat(GBLuserHomeDir, "/");
    }
}

void openConfigFile(char* configFilePathAndName)
{
    GBLsettingsDictionary = iniparser_load(configFilePathAndName);
    if(GBLsettingsDictionary == NULL)
    {
        printWarning("failed to open config file for reading, trying to create");
        
        int newConfigFile;

        newConfigFile = creat(configFilePathAndName, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if(newConfigFile <= 0)
        {
            printWarning("failed to create config file");
            return;
        }
        close(newConfigFile);
        
        GBLsettingsDictionary = iniparser_load(configFilePathAndName);
        if(GBLsettingsDictionary == NULL)
        {
            printWarning("iniparser failed to load the '.isomaster' just created, "
                         "this is a bug, but not critical (please do report it though).");
            return;
        }
    }
}

void getDefaultTempDir(char** destStr)
{
    *destStr = malloc(strlen(DEFAULT_TEMP_DIR) + 1);
    if(*destStr == NULL)
        fatalError("*destStr = malloc(strlen(DEFAULT_TEMP_DIR) +1) failed");
    strcpy(*destStr, DEFAULT_TEMP_DIR);
}

void loadSettings(void)
{
    char* configFileName;
    int width;
    int height;
    int showHidden;
    int sortDirsFirst;
    int scanForDuplicateFiles;
    int followSymLinks;
    char* tempStr;
    int appendExtension;
    int caseSensitiveSort;
    
    configFileName = malloc(strlen(GBLuserHomeDir) + strlen(".isomaster") + 1);
    if(configFileName == NULL)
        fatalError("loadSettings(): malloc(config file name) failed");
    
    strcpy(configFileName, GBLuserHomeDir);
    strcat(configFileName, ".isomaster");
    
    if(strcmp(GBLuserHomeDir, "/") != 0 && strcmp(GBLuserHomeDir, "c:\\") != 0)
        openConfigFile(configFileName);
    else
        printWarning("don't know user's home directory, so will not try to "
                     "load config file (~/.isomaster)");
    
    /* read/set window width */
    if(GBLsettingsDictionary != NULL)
    {
        width = iniparser_getint(GBLsettingsDictionary, "ui:windowwidth", INT_MAX);
        if(width == INT_MAX)
        /* not found in config file */
            GBLappSettings.windowWidth = ISOMASTER_DEFAULT_WINDOW_WIDTH;
        else
            GBLappSettings.windowWidth = width;
    }
    else
    /* no config file */
        GBLappSettings.windowWidth = ISOMASTER_DEFAULT_WINDOW_WIDTH;
    
    /* read/set window height */
    if(GBLsettingsDictionary != NULL)
    {
        height = iniparser_getint(GBLsettingsDictionary, "ui:windowheight", INT_MAX);
        if(height == INT_MAX)
        /* not found in config file */
            GBLappSettings.windowHeight = ISOMASTER_DEFAULT_WINDOW_HEIGHT;
        else
            GBLappSettings.windowHeight = height;
    }
    else
    /* no config file */
        GBLappSettings.windowHeight = ISOMASTER_DEFAULT_WINDOW_HEIGHT;
    
    /* read/set top pane height */
    if(GBLsettingsDictionary != NULL)
    {
        height = iniparser_getint(GBLsettingsDictionary, "ui:toppaneheight", INT_MAX);
        if(height == INT_MAX)
        /* not found in config file */
            GBLappSettings.topPaneHeight = ISOMASTER_DEFAULT_TOPPANE_HEIGHT;
        else
            GBLappSettings.topPaneHeight = height;
    }
    else
    /* no config file */
        GBLappSettings.topPaneHeight = ISOMASTER_DEFAULT_TOPPANE_HEIGHT;
    
    /* read/set fs browser directory */
    if(GBLsettingsDictionary != NULL)
    {
        /* pointer sharing is ok since GBLappSettings.fsCurrentDir is only written from here */
        GBLappSettings.fsCurrentDir = iniparser_getstring(GBLsettingsDictionary, 
                                                          "ui:fscurrentdir", NULL);
    }
    else
    /* no config file */
        GBLappSettings.fsCurrentDir = NULL;
    
    /* read/set show hidden files on filesystem */
    if(GBLsettingsDictionary != NULL)
    {
        showHidden = iniparser_getboolean(GBLsettingsDictionary, 
                                          "ui:showhiddenfilesfs", INT_MAX);
        if(showHidden == INT_MAX)
            GBLappSettings.showHiddenFilesFs = false;
        else
            GBLappSettings.showHiddenFilesFs = showHidden;
    }
    else
    /* no config file */
        GBLappSettings.showHiddenFilesFs = false;
    
    /* read/set sort directories first */
    if(GBLsettingsDictionary != NULL)
    {
        sortDirsFirst = iniparser_getboolean(GBLsettingsDictionary, 
                                             "ui:sortdirsfirst", INT_MAX);
        if(sortDirsFirst == INT_MAX)
            GBLappSettings.sortDirectoriesFirst = true;
        else
            GBLappSettings.sortDirectoriesFirst = sortDirsFirst;
    }
    else
    /* no config file */
        GBLappSettings.sortDirectoriesFirst = true;
    
    /* read/set scan for duplicate files */
    if(GBLsettingsDictionary != NULL)
    {
        scanForDuplicateFiles = iniparser_getboolean(GBLsettingsDictionary, 
                                                     "ui:scanforduplicatefiles", INT_MAX);
        if(scanForDuplicateFiles == INT_MAX)
            GBLappSettings.scanForDuplicateFiles = true;
        else
            GBLappSettings.scanForDuplicateFiles = scanForDuplicateFiles;
    }
    else
    /* no config file */
        GBLappSettings.scanForDuplicateFiles = true;
    
    /* read/set follow symbolic links */
    if(GBLsettingsDictionary != NULL)
    {
        followSymLinks = iniparser_getboolean(GBLsettingsDictionary, 
                                              "ui:followsymlinks", INT_MAX);
        if(followSymLinks == INT_MAX)
            GBLappSettings.followSymLinks = false;
        else
            GBLappSettings.followSymLinks = followSymLinks;
    }
    else
    /* no config file */
        GBLappSettings.followSymLinks = false;
    
    /* read/set last iso open/save dir */
    if(GBLsettingsDictionary != NULL)
    {
        tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                      "ui:lastisodir", NULL);
        if(tempStr == NULL)
            GBLappSettings.lastIsoDir = NULL;
        else
        {
            GBLappSettings.lastIsoDir = malloc(strlen(tempStr) +1);
            if(GBLappSettings.lastIsoDir == NULL)
                fatalError("GBLappSettings.lastIsoDir = malloc(strlen(tempStr) +1) failed");
            strcpy(GBLappSettings.lastIsoDir, tempStr);
        }
    }
    else
    /* no config file */
        GBLappSettings.lastIsoDir = NULL;
    
    /* read/set last boot record dir */
    if(GBLsettingsDictionary != NULL)
    {
        tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                      "ui:lastbootrecorddir", NULL);
        if(tempStr == NULL)
            GBLappSettings.lastBootRecordDir = NULL;
        else
        {
            GBLappSettings.lastBootRecordDir = malloc(strlen(tempStr) +1);
            if(GBLappSettings.lastBootRecordDir == NULL)
                fatalError("GBLappSettings.lastBootRecordDir = malloc(strlen(tempStr) +1) failed");
            strcpy(GBLappSettings.lastBootRecordDir, tempStr);
        }
    }
    else
    /* no config file */
        GBLappSettings.lastBootRecordDir = NULL;
    
    /* read/set append extension */
    if(GBLsettingsDictionary != NULL)
    {
        appendExtension = iniparser_getboolean(GBLsettingsDictionary, 
                                              "ui:appendextension", INT_MAX);
        if(showHidden == INT_MAX)
            GBLappSettings.appendExtension = true;
        else
            GBLappSettings.appendExtension = appendExtension;
    }
    else
    /* no config file */
        GBLappSettings.appendExtension = true;
    
    /* read/set editor */
    if(GBLsettingsDictionary != NULL)
    {
        tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                      "ui:editor", NULL);
        if(tempStr == NULL)
        {
            GBLappSettings.editor = malloc(strlen(DEFAULT_EDITOR) + 1);
            if(GBLappSettings.editor == NULL)
                fatalError("GBLappSettings.editor = malloc(strlen(DEFAULT_EDITOR) +1) failed");
            strcpy(GBLappSettings.editor, DEFAULT_EDITOR);
        }
        else
        {
            GBLappSettings.editor = malloc(strlen(tempStr) + 1);
            if(GBLappSettings.editor == NULL)
                fatalError("GBLappSettings.editor = malloc(strlen(tempStr) + 1) failed");
            strcpy(GBLappSettings.editor, tempStr);
        }
    }
    else
    /* no config file */
    {
        GBLappSettings.editor = malloc(strlen(DEFAULT_EDITOR) + 1);
        if(GBLappSettings.editor == NULL)
            fatalError("GBLappSettings.editor = malloc(strlen(DEFAULT_EDITOR) +1) failed");
        strcpy(GBLappSettings.editor, DEFAULT_EDITOR);
    }
    
    /* read/set viewer */
    if(GBLsettingsDictionary != NULL)
    {
        tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                      "ui:viewer", NULL);
        if(tempStr == NULL)
        {
            GBLappSettings.viewer = malloc(strlen(DEFAULT_VIEWER) + 1);
            if(GBLappSettings.viewer == NULL)
                fatalError("GBLappSettings.viewer = malloc(strlen(DEFAULT_VIEWER) +1) failed");
            strcpy(GBLappSettings.viewer, DEFAULT_VIEWER);
        }
        else
        {
            GBLappSettings.viewer = malloc(strlen(tempStr) + 1);
            if(GBLappSettings.viewer == NULL)
                fatalError("GBLappSettings.viewer = malloc(strlen(tempStr) + 1) failed");
            strcpy(GBLappSettings.viewer, tempStr);
        }
    }
    else
    /* no config file */
    {
        GBLappSettings.viewer = malloc(strlen(DEFAULT_VIEWER) + 1);
        if(GBLappSettings.viewer == NULL)
            fatalError("GBLappSettings.viewer = malloc(strlen(DEFAULT_VIEWER) +1) failed");
        strcpy(GBLappSettings.viewer, DEFAULT_VIEWER);
    }
    
    /* read/set temporary directory */
    if(GBLsettingsDictionary != NULL)
    {
        tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                      "ui:tempdir", NULL);
        if(tempStr == NULL)
        {
            getDefaultTempDir( &(GBLappSettings.tempDir) );
        }
        else
        {
            GBLappSettings.tempDir = malloc(strlen(tempStr) + 1);
            if(GBLappSettings.tempDir == NULL)
                fatalError("GBLappSettings.tempDir = malloc(strlen(tempStr) + 1) failed");
            strcpy(GBLappSettings.tempDir, tempStr);
        }
    }
    else
    /* no config file */
    {
        getDefaultTempDir( &(GBLappSettings.tempDir) );
    }
    
    /* read/set iso sort column id */
    if(GBLsettingsDictionary != NULL)
    {
        GBLappSettings.isoSortColumnId = iniparser_getint(GBLsettingsDictionary, 
                                                          "ui:isosortcolumnid", INT_MAX);
        if(GBLappSettings.isoSortColumnId == INT_MAX)
            GBLappSettings.isoSortColumnId = COLUMN_FILENAME;
    }
    else
    /* no config file */
        GBLappSettings.isoSortColumnId = COLUMN_FILENAME;
    
    /* read/set iso sort direction */
    if(GBLsettingsDictionary != NULL)
    {
        GBLappSettings.isoSortDirection = iniparser_getint(GBLsettingsDictionary, 
                                                          "ui:isosortdirection", INT_MAX);
        if(GBLappSettings.isoSortDirection == INT_MAX)
            GBLappSettings.isoSortDirection = GTK_SORT_ASCENDING;
    }
    else
    /* no config file */
        GBLappSettings.isoSortDirection = GTK_SORT_ASCENDING;
    
    /* read/set fs sort column id */
    if(GBLsettingsDictionary != NULL)
    {
        GBLappSettings.fsSortColumnId = iniparser_getint(GBLsettingsDictionary, 
                                                          "ui:fssortcolumnid", INT_MAX);
        if(GBLappSettings.fsSortColumnId == INT_MAX)
            GBLappSettings.fsSortColumnId = COLUMN_FILENAME;
    }
    else
    /* no config file */
        GBLappSettings.fsSortColumnId = COLUMN_FILENAME;
    
    /* read/set fs sort direction */
    if(GBLsettingsDictionary != NULL)
    {
        GBLappSettings.fsSortDirection = iniparser_getint(GBLsettingsDictionary, 
                                                          "ui:fssortdirection", INT_MAX);
        if(GBLappSettings.fsSortDirection == INT_MAX)
            GBLappSettings.fsSortDirection = GTK_SORT_ASCENDING;
    }
    else
    /* no config file */
        GBLappSettings.fsSortDirection = GTK_SORT_ASCENDING;
    
    /* read/set recently open files */
    for(int i = 0; i < 5; i++)
    {
        GBLappSettings.recentlyOpen[i] = NULL;
        if(GBLsettingsDictionary != NULL)
        {
            char configNameStr[20] = "ui:recentlyopen";
            snprintf(configNameStr + 15, 5, "%d", i);
            
            tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                          configNameStr, NULL);
            if(tempStr != NULL)
            {
                GBLappSettings.recentlyOpen[i] = malloc(strlen(tempStr) + 1);
                if(GBLappSettings.recentlyOpen[i] == NULL)
                    fatalError("GBLappSettings.recentlyOpen1 = malloc(strlen(tempStr) + 1) failed");
                strcpy(GBLappSettings.recentlyOpen[i], tempStr);
            }
        }
    }
    
    /* read/set fs drive */
    GBLappSettings.fsDrive = malloc(4); /* "c:\\" */
    if(GBLappSettings.fsDrive == NULL)
        fatalError("GBLappSettings.fsDrive = malloc(5) failed");
    if(GBLsettingsDictionary != NULL)
    {
        tempStr = iniparser_getstring(GBLsettingsDictionary, 
                                      "ui:fsdrive", NULL);
        if(tempStr == NULL || tempStr[1] != ':' || tempStr[2] != '\\' || tempStr[3] != '\0')
        {
            strcpy(GBLappSettings.fsDrive, "c:\\");
        }
        else
        {
            strcpy(GBLappSettings.fsDrive, tempStr);
        }
    }
    else
    /* no config file */
    {
        strcpy(GBLappSettings.fsDrive, "c:\\");
    }
    
    /* read/set case sensitive sort */
    if(GBLsettingsDictionary != NULL)
    {
        caseSensitiveSort = iniparser_getboolean(GBLsettingsDictionary, 
                                                 "ui:casesensitivesort", INT_MAX);
        if(caseSensitiveSort == INT_MAX)
            GBLappSettings.caseSensitiveSort = true;
        else
            GBLappSettings.caseSensitiveSort = caseSensitiveSort;
    }
    else
    /* no config file */
        GBLappSettings.caseSensitiveSort = true;
    
    free(configFileName);
}

void scanForDuplicatesCbk(GtkButton* button, gpointer data)
{
    if(GBLisoPaneActive)
    {
        GtkWidget* warningDialog;
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_WARNING,
                                               GTK_BUTTONS_CLOSE,
                                               _("You will have to close and reopen the ISO for"
                                               " this change to take effect"));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
    }
}

void showPreferencesWindowCbk(GtkButton* button, gpointer data)
{
    int rc;
    static PrefWidgets prefWidgets;
    GtkWidget* label;
    
    prefWidgets.dialog = gtk_dialog_new_with_buttons(_("Options"),
                                         GTK_WINDOW(GBLmainWindow),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    gtk_window_set_transient_for(GTK_WINDOW(prefWidgets.dialog), GTK_WINDOW(GBLmainWindow));
    gtk_window_set_modal(GTK_WINDOW(prefWidgets.dialog), TRUE);
    
    prefWidgets.scanForDuplicateFiles = gtk_check_button_new_with_mnemonic(
                                            _("Scan for duplicate files (slow)"));
    if(GBLappSettings.scanForDuplicateFiles)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefWidgets.scanForDuplicateFiles), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefWidgets.scanForDuplicateFiles), FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), 
                       prefWidgets.scanForDuplicateFiles, 
                       TRUE, TRUE, 0);
    gtk_widget_show(prefWidgets.scanForDuplicateFiles);
    g_signal_connect(G_OBJECT(prefWidgets.scanForDuplicateFiles), "activate",
                     G_CALLBACK(scanForDuplicatesCbk), NULL);
    
    prefWidgets.followSymlinks = gtk_check_button_new_with_mnemonic(
                                    _("Follow symbolic links"));
    if(GBLappSettings.followSymLinks)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefWidgets.followSymlinks), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(prefWidgets.followSymlinks), FALSE);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), 
                       prefWidgets.followSymlinks, 
                       TRUE, TRUE, 0);
    gtk_widget_show(prefWidgets.followSymlinks);
    
    label = gtk_label_new(_("Editor"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), 
                       label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    prefWidgets.editor = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(prefWidgets.editor), GBLappSettings.editor);
    gtk_entry_set_width_chars(GTK_ENTRY(prefWidgets.editor), 30);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), 
                       prefWidgets.editor, TRUE, TRUE, 0);
    gtk_widget_show(prefWidgets.editor);
    
    label = gtk_label_new(_("Viewer"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    prefWidgets.viewer = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(prefWidgets.viewer), GBLappSettings.viewer);
    gtk_entry_set_width_chars(GTK_ENTRY(prefWidgets.viewer), 30);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), 
                       prefWidgets.viewer, TRUE, TRUE, 0);
    gtk_widget_show(prefWidgets.viewer);
    
    label = gtk_label_new(_("Temporary directory"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    prefWidgets.tempDir = gtk_file_chooser_button_new(_("Temporary directory"), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(prefWidgets.tempDir), GBLappSettings.tempDir);
    //!! add /tmp to quicklist or something
    gtk_file_chooser_add_shortcut_folder(GTK_FILE_CHOOSER(prefWidgets.tempDir), "/tmp", NULL);
    gtk_box_pack_start (GTK_BOX(GTK_DIALOG(prefWidgets.dialog)->vbox), prefWidgets.tempDir, 
        TRUE, TRUE, 0);
    gtk_widget_show (prefWidgets.tempDir);
    
    rc = gtk_dialog_run(GTK_DIALOG(prefWidgets.dialog));
    if(rc == GTK_RESPONSE_ACCEPT)
    {
        GBLappSettings.scanForDuplicateFiles = 
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prefWidgets.scanForDuplicateFiles));
        
        GBLappSettings.followSymLinks = 
                gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(prefWidgets.followSymlinks));
        bk_set_follow_symlinks(&GBLvolInfo, GBLappSettings.followSymLinks);
        
        if(GBLappSettings.editor != NULL)
            free(GBLappSettings.editor);
        GBLappSettings.editor = malloc(strlen(gtk_entry_get_text(GTK_ENTRY(prefWidgets.editor))) + 1);
        if(GBLappSettings.editor == NULL)
            fatalError("GBLappSettings.editor = malloc(...) failed");
        strcpy(GBLappSettings.editor, gtk_entry_get_text(GTK_ENTRY(prefWidgets.editor)));
        
        if(GBLappSettings.viewer != NULL)
            free(GBLappSettings.viewer);
        GBLappSettings.viewer = malloc(strlen(gtk_entry_get_text(GTK_ENTRY(prefWidgets.viewer))) + 1);
        if(GBLappSettings.viewer == NULL)
            fatalError("GBLappSettings.viewer = malloc(...) failed");
        strcpy(GBLappSettings.viewer, gtk_entry_get_text(GTK_ENTRY(prefWidgets.viewer)));
        
        if(GBLappSettings.tempDir != NULL)
            free(GBLappSettings.tempDir);
        GBLappSettings.tempDir = malloc(strlen(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(prefWidgets.tempDir))) + 1);
        if(GBLappSettings.tempDir == NULL)
            fatalError("GBLappSettings.tempDir = malloc(...) failed");
        strcpy(GBLappSettings.tempDir, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(prefWidgets.tempDir)));
    }
    
    gtk_widget_destroy(prefWidgets.dialog);
}

void writeSettings(void)
{
    char* configFileName;
    FILE* fileToWrite;
    char numberStr[20];
    int width;
    int height;
    int sortColumnId;
    GtkSortType sortDirection;
    
    if(strcmp(GBLuserHomeDir, "/") == 0 || strcmp(GBLuserHomeDir, "c:\\") == 0)
    {
        printWarning("don't know user's home directory, so will not try to save "
                     "config file");
        return;
    }
    if(GBLsettingsDictionary == NULL)
    {
        printWarning("failed to create config file when app started, "
                     "will not try again, settings not saved");
        return;
    }
    
    configFileName = malloc(strlen(GBLuserHomeDir) + strlen(".isomaster") + 1);
    if(configFileName == NULL)
        fatalError("writeSettings(): malloc(config file name) failed");
    
    strcpy(configFileName, GBLuserHomeDir);
    strcat(configFileName, ".isomaster");
    
    fileToWrite = fopen(configFileName, "w");
    if(fileToWrite == NULL)
    {
        printWarning("could not open ~/.isomaster for writing, config not saved");
        free(configFileName);
        return;
    }
    
    free(configFileName);
    
    iniparser_setstr(GBLsettingsDictionary, "ui", NULL);
    
    gtk_window_get_size(GTK_WINDOW(GBLmainWindow), &width, &height);
    snprintf(numberStr, 20, "%d", width);
    iniparser_setstr(GBLsettingsDictionary, "ui:windowwidth", numberStr);
    snprintf(numberStr, 20, "%d", height);
    iniparser_setstr(GBLsettingsDictionary, "ui:windowheight", numberStr);
    
    height = gtk_paned_get_position(GTK_PANED(GBLbrowserPaned));
    snprintf(numberStr, 20, "%d", height);
    iniparser_setstr(GBLsettingsDictionary, "ui:toppaneheight", numberStr);
    
    iniparser_setstr(GBLsettingsDictionary, "ui:fscurrentdir", GBLfsCurrentDir);
    
    snprintf(numberStr, 20, "%d", GBLappSettings.showHiddenFilesFs);
    iniparser_setstr(GBLsettingsDictionary, "ui:showhiddenfilesfs", numberStr);
    
    snprintf(numberStr, 20, "%d", GBLappSettings.sortDirectoriesFirst);
    iniparser_setstr(GBLsettingsDictionary, "ui:sortdirsfirst", numberStr);
    
    snprintf(numberStr, 20, "%d", GBLappSettings.scanForDuplicateFiles);
    iniparser_setstr(GBLsettingsDictionary, "ui:scanforduplicatefiles", numberStr);
    
    snprintf(numberStr, 20, "%d", GBLappSettings.followSymLinks);
    iniparser_setstr(GBLsettingsDictionary, "ui:followsymlinks", numberStr);
    
    if(GBLappSettings.lastIsoDir != NULL)
        iniparser_setstr(GBLsettingsDictionary, "ui:lastisodir", GBLappSettings.lastIsoDir);
    
    if(GBLappSettings.lastBootRecordDir != NULL)
        iniparser_setstr(GBLsettingsDictionary, "ui:lastbootrecorddir", GBLappSettings.lastBootRecordDir);
    
    snprintf(numberStr, 20, "%d", GBLappSettings.appendExtension);
    iniparser_setstr(GBLsettingsDictionary, "ui:appendextension", numberStr);
    
    iniparser_setstr(GBLsettingsDictionary, "ui:editor", GBLappSettings.editor);
    
    iniparser_setstr(GBLsettingsDictionary, "ui:viewer", GBLappSettings.viewer);
    
    iniparser_setstr(GBLsettingsDictionary, "ui:tempdir", GBLappSettings.tempDir);
    
    gtk_tree_sortable_get_sort_column_id(GTK_TREE_SORTABLE(GBLisoListStore), 
                                         &sortColumnId, &sortDirection);
    snprintf(numberStr, 20, "%d", sortColumnId);
    iniparser_setstr(GBLsettingsDictionary, "ui:isosortcolumnid", numberStr);
    snprintf(numberStr, 20, "%d", sortDirection);
    iniparser_setstr(GBLsettingsDictionary, "ui:isosortdirection", numberStr);
    
    gtk_tree_sortable_get_sort_column_id(GTK_TREE_SORTABLE(GBLfsListStore), 
                                         &sortColumnId, &sortDirection);
    snprintf(numberStr, 20, "%d", sortColumnId);
    iniparser_setstr(GBLsettingsDictionary, "ui:fssortcolumnid", numberStr);
    snprintf(numberStr, 20, "%d", sortDirection);
    iniparser_setstr(GBLsettingsDictionary, "ui:fssortdirection", numberStr);
    
    iniparser_setstr(GBLsettingsDictionary, "ui:fsdrive", GBLappSettings.fsDrive);
    
    snprintf(numberStr, 20, "%d", GBLappSettings.caseSensitiveSort);
    iniparser_setstr(GBLsettingsDictionary, "ui:casesensitivesort", numberStr);
    
    for(int i = 0; i < 5; i++)
    {
        char configNameStr[20] = "ui:recentlyopen";
        snprintf(configNameStr + 15, 5, "%d", i);
        
        iniparser_setstr(GBLsettingsDictionary, configNameStr, 
            gtk_label_get_text(GTK_LABEL(
                gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[i])))));
    }
    
    iniparser_dump_ini(GBLsettingsDictionary, fileToWrite);
}
