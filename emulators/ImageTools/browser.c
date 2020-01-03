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

#include <gtk/gtk.h>
#include <string.h>
#include <sys/stat.h>
#include <libintl.h>

#include "isomaster.h"

/* this file has thigs shared by the fs and the iso browser */

/* menu-sized pixbufs of a directory and a file */
GdkPixbuf* GBLdirPixbuf;
GdkPixbuf* GBLfilePixbuf;
//~ GdkPixbuf* GBLsymlinkPixbuf;

/* text box for showing the path and name of the current directory on the fs */
GtkWidget* GBLfsCurrentDirField;
/* the view used for the contents of the fs browser */
GtkWidget* GBLfsTreeView;
/* the list store used for the contents of the fs browser */
GtkListStore* GBLfsListStore;
/* slash-terminated, the dir being displayed in the fs browser */
char* GBLfsCurrentDir = NULL;

/* text box for showing the path and name of the current directory on the iso */
GtkWidget* GBLisoCurrentDirField;
/* the view used for the contents of the fs browser */
GtkWidget* GBLisoTreeView;
/* the list store used for the contents of the fs browser */
GtkListStore* GBLisoListStore;
/* slash-terminated, the dir being displayed in the iso browser */
char* GBLisoCurrentDir = NULL;

extern GtkWidget* GBLmainWindow;
extern VolInfo GBLvolInfo;
extern bool GBLisoPaneActive;
extern AppSettings GBLappSettings;

/* connected to the activate signal of a text entry in a dialog */
void acceptDialogCbk(GtkEntry *entry, GtkDialog* dialog)
{
    gtk_dialog_response(dialog, GTK_RESPONSE_ACCEPT);
}

void createDirCbk(GtkButton *button, gpointer onFs)
{
    GtkWidget* dialog;
    GtkWidget* warningDialog;
    int response;
    GtkWidget* textEntry;
    const char* newDirName;
    int rc;
    
    if(!onFs && !GBLisoPaneActive)
    /* asked to create dir on iso but no iso is open */
        return;
    
    dialog = gtk_dialog_new_with_buttons(_("Enter name for new directory"),
                                         GTK_WINDOW(GBLmainWindow),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    
    textEntry = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(textEntry), 40);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), textEntry);
    gtk_widget_show(textEntry);
    g_signal_connect(textEntry, "activate", (GCallback)acceptDialogCbk, dialog);
    g_signal_connect(dialog, "close", G_CALLBACK(rejectDialogCbk), NULL);
    
    response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if(response == GTK_RESPONSE_ACCEPT)
    {
        newDirName = gtk_entry_get_text(GTK_ENTRY(textEntry));
        
        if(onFs)
        {
            char* pathAndName;
            
            pathAndName = malloc(strlen(GBLfsCurrentDir) + strlen(newDirName) + 1);
            if(pathAndName == NULL)
                fatalError("createDirCbk(): malloc(strlen(GBLfsCurrentDir) + "
                           "strlen(newDirName) + 1) failed");
            
            strcpy(pathAndName, GBLfsCurrentDir);
            strcat(pathAndName, newDirName);
            
            rc = mkdir(pathAndName, 0755);
            if(rc == -1)
            {
                warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_CLOSE,
                                                       _("Failed to create directory %s"),
                                                       pathAndName);
                gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
                gtk_dialog_run(GTK_DIALOG(warningDialog));
                gtk_widget_destroy(warningDialog);
                gtk_widget_destroy(dialog);
                return;
            }
            
            free(pathAndName);
            
            refreshFsView();
        }
        else
        /* on iso */
        {
            rc = bk_create_dir(&GBLvolInfo, GBLisoCurrentDir, newDirName);
            if(rc <= 0)
            {
                warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                       GTK_DIALOG_DESTROY_WITH_PARENT,
                                                       GTK_MESSAGE_ERROR,
                                                       GTK_BUTTONS_CLOSE,
                                                       _("Failed to create directory %s: '%s'"),
                                                       newDirName,
                                                       bk_get_error_string(rc));
                gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
                gtk_dialog_run(GTK_DIALOG(warningDialog));
                gtk_widget_destroy(warningDialog);
                gtk_widget_destroy(dialog);
                return;
            }
            
            refreshIsoView();
        }
        fflush(NULL);
    }
    
    gtk_widget_destroy(dialog);
}

void formatSize(bk_off_t sizeInt, char* sizeStr, int sizeStrLen)
{
    if(sizeInt > 1073741824)
    /* print gibibytes */
        snprintf(sizeStr, sizeStrLen, "%.1f GB", (double)sizeInt / 1073741824);
    else if(sizeInt > 1048576)
    /* print mebibytes */
        snprintf(sizeStr, sizeStrLen, "%.1f MB", (double)sizeInt / 1048576);
    else if(sizeInt > 1024)
    /* print kibibytes */
        snprintf(sizeStr, sizeStrLen, "%.1f KB", (double)sizeInt / 1024);
    else
    /* print bytes */
        snprintf(sizeStr, sizeStrLen, "%llu B", sizeInt);
    
    sizeStr[sizeStrLen - 1] = '\0';
}

void refreshBothViewsCbk(GtkWidget *widget, GdkEvent *event)
{
    refreshFsView();
    if(GBLisoPaneActive)
        refreshIsoView();
}

/* formats the file size text for displaying */
void sizeCellDataFunc32(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                        GtkTreeModel *model, GtkTreeIter *iter,
                        gpointer data)
{
    unsigned sizeInt;
    unsigned long long sizeLlInt;
    int fileType;
    char sizeStr[20];
    
    gtk_tree_model_get(model, iter, COLUMN_SIZE, &sizeInt, 
                                    COLUMN_HIDDEN_TYPE, &fileType, -1);
    
    sizeLlInt = sizeInt;
    
    if(fileType == FILE_TYPE_DIRECTORY)
    {
        snprintf(sizeStr, sizeof(sizeStr), "dir");
        sizeStr[sizeof(sizeStr) - 1] = '\0';
    }
    else if(fileType == FILE_TYPE_SYMLINK)
    {
        snprintf(sizeStr, sizeof(sizeStr), "link");
        sizeStr[sizeof(sizeStr) - 1] = '\0';
    }
    else
        formatSize(sizeLlInt, sizeStr, sizeof(sizeStr));
    
    g_object_set(renderer, "text", sizeStr, NULL);
}

/* formats the file size text for displaying */
void sizeCellDataFunc64(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                        GtkTreeModel *model, GtkTreeIter *iter,
                        gpointer data)
{
    unsigned long long sizeInt;
    int fileType;
    char sizeStr[20];
    
    gtk_tree_model_get(model, iter, COLUMN_SIZE, &sizeInt, 
                                    COLUMN_HIDDEN_TYPE, &fileType, -1);
    
    if(fileType == FILE_TYPE_DIRECTORY)
    {
        snprintf(sizeStr, sizeof(sizeStr), "dir");
        sizeStr[sizeof(sizeStr) - 1] = '\0';
    }
    else if(fileType == FILE_TYPE_SYMLINK)
    {
        snprintf(sizeStr, sizeof(sizeStr), "link");
        sizeStr[sizeof(sizeStr) - 1] = '\0';
    }
    else
        formatSize(sizeInt, sizeStr, sizeof(sizeStr));
    
    g_object_set(renderer, "text", sizeStr, NULL);
}

gint sortByName(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata)
{
    if(GBLappSettings.sortDirectoriesFirst)
    /* directories before files */
    {
        int aFileType;
        int bFileType;
        gint unused;
        GtkSortType order;
        
        gtk_tree_model_get(model, a, COLUMN_HIDDEN_TYPE, &aFileType, -1);
        gtk_tree_model_get(model, b, COLUMN_HIDDEN_TYPE, &bFileType, -1);
        
        /* have to make sure directories come first regardless of sort order, 
        * that's why all the fancyness in the rest of this block */
        gtk_tree_sortable_get_sort_column_id(GTK_TREE_SORTABLE(model), &unused, &order);
        
        if(aFileType == FILE_TYPE_DIRECTORY && bFileType != FILE_TYPE_DIRECTORY)
        {
            if(order == GTK_SORT_ASCENDING)
                return -1;
            else
                return 1;
        }
        else if(aFileType != FILE_TYPE_DIRECTORY && bFileType == FILE_TYPE_DIRECTORY)
        {
            if(order == GTK_SORT_ASCENDING)
                return 1;
            else
                return -1;
        }
    }
    
    char* aName;
    char* bName;
    gint toReturn;
    
    gtk_tree_model_get(model, a, COLUMN_FILENAME, &aName, -1);
    gtk_tree_model_get(model, b, COLUMN_FILENAME, &bName, -1);
    
    if(GBLappSettings.caseSensitiveSort)
        toReturn = strcmp(aName, bName);
    else
        toReturn = strcasecmp(aName, bName);
    
    g_free(aName);
    g_free(bName);
    
    return toReturn;
}

gint sortBySize(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata)
{
    if(GBLappSettings.sortDirectoriesFirst)
    /* directories before files */
    {
        int aFileType;
        int bFileType;
        gint unused;
        GtkSortType order;
        
        gtk_tree_model_get(model, a, COLUMN_HIDDEN_TYPE, &aFileType, -1);
        gtk_tree_model_get(model, b, COLUMN_HIDDEN_TYPE, &bFileType, -1);
        
        /* have to make sure directories come first regardless of sort order, 
        * that's why all the fancyness below */
        gtk_tree_sortable_get_sort_column_id(GTK_TREE_SORTABLE(model), &unused, &order);
        
        if(aFileType == FILE_TYPE_DIRECTORY && bFileType != FILE_TYPE_DIRECTORY)
        {
            if(order == GTK_SORT_ASCENDING)
                return -1;
            else
                return 1;
        }
        else if(aFileType != FILE_TYPE_DIRECTORY && bFileType == FILE_TYPE_DIRECTORY)
        {
            if(order == GTK_SORT_ASCENDING)
                return 1;
            else
                return -1;
        }
    }
    
    guint64 aSize;
    guint64 bSize;
    
    gtk_tree_model_get(model, a, COLUMN_SIZE, &aSize, -1);
    gtk_tree_model_get(model, b, COLUMN_SIZE, &bSize, -1);
    
    if(aSize < bSize)
        return -1;
    else
        return 1;
}

/* this function exists to deal with the gtk stupidity that sorting can't be disabled */
gint sortVoid(GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata)
{
    return 0;
}
