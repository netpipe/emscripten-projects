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
#include <libintl.h>

#include "isomaster.h"

extern GtkWidget* GBLmainWindow;
extern VolInfo GBLvolInfo;
extern bool GBLisoPaneActive;
extern GtkWidget* GBLisoTreeView;
extern char* GBLisoCurrentDir;
extern AppSettings GBLappSettings;
extern bool GBLisoChangesProbable;

void addBootRecordFromFileCbk(GtkButton *button, gpointer bootRecordType)
{
    GtkWidget* dialog;
    GtkWidget* warningDialog;
    char* filename;
    int rc;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    dialog = gtk_file_chooser_dialog_new(_("Choose Boot Record File"),
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);
    
    if(GBLappSettings.lastBootRecordDir != NULL)
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), GBLappSettings.lastBootRecordDir);
    
    if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        /* RECORD last boot record dir */
        char* lastBootRecordDir = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        
        if(GBLappSettings.lastBootRecordDir != NULL && 
           strlen(lastBootRecordDir) > strlen(GBLappSettings.lastBootRecordDir))
        {
            free(GBLappSettings.lastBootRecordDir);
            GBLappSettings.lastBootRecordDir = NULL;
        }
        
        if(GBLappSettings.lastBootRecordDir == NULL)
            GBLappSettings.lastBootRecordDir = malloc(strlen(lastBootRecordDir) + 1);
        
        strcpy(GBLappSettings.lastBootRecordDir, lastBootRecordDir);
        
        g_free(lastBootRecordDir);
        /* END RECORD last boot record dir */
        
        rc = bk_add_boot_record(&GBLvolInfo, filename, (int)bootRecordType);
        if(rc <= 0)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Failed to add boot record: '%s'"),
                                                   bk_get_error_string(rc));
            gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        else
            GBLisoChangesProbable = true;
        
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

void deleteBootRecordCbk(GtkButton *button, gpointer data)
{
    GtkWidget* dialog;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    if(GBLvolInfo.bootMediaType == BOOT_MEDIA_NONE)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("No boot to delete"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    else
    {
        bk_delete_boot_record(&GBLvolInfo);
        
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_CLOSE,
                                        _("Boot record deleted"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        GBLisoChangesProbable = true;
    }
}

void extractBootRecordCbk(GtkButton *button, gpointer data)
{
    GtkWidget* dialog;
    GtkWidget* warningDialog;
    int dialogResponse;
    char* filename = NULL;
    int rc;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    if(GBLvolInfo.bootMediaType == BOOT_MEDIA_NONE)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("No boot record read from original or set on image"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    dialog = gtk_file_chooser_dialog_new(_("Save a copy of the boot record"),
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                         NULL);
    
    if(GBLappSettings.lastBootRecordDir != NULL)
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), GBLappSettings.lastBootRecordDir);
    
    dialogResponse = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if(dialogResponse == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        /* RECORD last boot record dir */
        char* lastBootRecordDir = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        
        if(GBLappSettings.lastBootRecordDir != NULL && 
           strlen(lastBootRecordDir) > strlen(GBLappSettings.lastBootRecordDir))
        {
            free(GBLappSettings.lastBootRecordDir);
            GBLappSettings.lastBootRecordDir = NULL;
        }
        
        if(GBLappSettings.lastBootRecordDir == NULL)
            GBLappSettings.lastBootRecordDir = malloc(strlen(lastBootRecordDir) + 1);
        
        strcpy(GBLappSettings.lastBootRecordDir, lastBootRecordDir);
        
        g_free(lastBootRecordDir);
        /* END RECORD last boot record dir */
    }
    
    gtk_widget_destroy(dialog);
    
    if(dialogResponse == GTK_RESPONSE_ACCEPT)
    {
        rc = bk_extract_boot_record(&GBLvolInfo, filename, 0644);
        if(rc <= 0)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Failed to extract boot record: '%s'"),
                                                   bk_get_error_string(rc));
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        
        g_free(filename);
    }
}

void setFileAsBootRecordCbk(GtkButton *button, gpointer data)
{
    GtkTreeSelection* selection;
    GtkWidget* dialog;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    
    if(gtk_tree_selection_count_selected_rows(selection) == 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("Please select a file in the ISO browser to "
                                        "use as the boot record"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    if(gtk_tree_selection_count_selected_rows(selection) > 1)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("Please select no more than one file in the ISO browser"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    gtk_tree_selection_selected_foreach(selection, setFileAsBootRecordRowCbk, NULL);
}

void setFileAsBootRecordRowCbk(GtkTreeModel* model, GtkTreePath* path,
                               GtkTreeIter* iterator, gpointer data)
{
    int fileType;
    char* itemName;
    char* fullItemName;
    GtkWidget* dialog;
    int rc;
    
    gtk_tree_model_get(model, iterator, COLUMN_HIDDEN_TYPE, &fileType, 
                                        COLUMN_FILENAME, &itemName, -1);
    
    if(fileType != FILE_TYPE_REGULAR)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("Item selected is not a regular file and cannot"
                                        " be used as a boot record"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        g_free(itemName);
        return;
    }
    
    fullItemName = (char*)malloc(strlen(GBLisoCurrentDir) + strlen(itemName) + 1);
    if(fullItemName == NULL)
        fatalError("setFileAsBootRecordRowCbk(): malloc("
                   "strlen(GBLisoCurrentDir) + strlen(itemName) + 1) failed");
    
    strcpy(fullItemName, GBLisoCurrentDir);
    strcat(fullItemName, itemName);
    
    rc = bk_set_boot_file(&GBLvolInfo, fullItemName);
    if(rc <= 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        _("Failed to set %s as boot record: '%s'"),
                                        itemName,
                                        bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    else
        GBLisoChangesProbable = true;
    
    g_free(itemName);
}

void showBootInfoCbk(GtkButton *button, gpointer data)
{
    GtkWidget* dialog;
    GtkWidget* vBox;
    //~ GtkWidget* hBox;
    GtkWidget* label;
    char str[100];
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    if(GBLvolInfo.bootMediaType == BOOT_MEDIA_NONE)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_INFO,
                                        GTK_BUTTONS_CLOSE,
                                        _("No boot record read from original or set on image"));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    dialog = gtk_dialog_new_with_buttons(_("Boot Record Information"),
                                         GTK_WINDOW(GBLmainWindow),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_NONE,
                                         NULL);
    //gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    g_signal_connect(dialog, "close", G_CALLBACK(rejectDialogCbk), NULL);
    
    vBox = gtk_vbox_new(TRUE, 5);
    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(dialog)->vbox), vBox);
    gtk_widget_show(vBox);
    
    if(GBLvolInfo.bootMediaType == BOOT_MEDIA_NO_EMULATION)
        sprintf(str, _("Boot record type: No Emulation"));
    else if(GBLvolInfo.bootMediaType == BOOT_MEDIA_1_2_FLOPPY)
        sprintf(str, _("Boot record type: 1200KiB Floppy"));
    else if(GBLvolInfo.bootMediaType == BOOT_MEDIA_1_44_FLOPPY)
        sprintf(str, _("Boot record type: 1440KiB Floppy"));
    else if(GBLvolInfo.bootMediaType == BOOT_MEDIA_2_88_FLOPPY)
        sprintf(str, _("Boot record type: 2880KiB Floppy"));
    else
        sprintf(str, "Boot record type: error");
    label = gtk_label_new(str);
    gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    if(GBLvolInfo.bootMediaType == BOOT_MEDIA_NO_EMULATION)
    {
        snprintf(str, 100, _("Size: %d bytes"), GBLvolInfo.bootRecordSize);
        label = gtk_label_new(str);
        gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 0);
        gtk_widget_show(label);
    }
    
    if(GBLvolInfo.bootRecordIsVisible)
    /* get this info from the original file */
    {
        if(GBLvolInfo.bootRecordOnImage->onImage)
            snprintf(str, 100, _("Location: on original image at 0x%llX"), GBLvolInfo.bootRecordOnImage->position);
        else
            snprintf(str, 100, _("Location: to be added from '%s'"), GBLvolInfo.bootRecordOnImage->pathAndName);
    }
    else
    {
        if(GBLvolInfo.bootRecordIsOnImage)
            snprintf(str, 100, _("Location: on original image at 0x%llX"), GBLvolInfo.bootRecordOffset);
        else
            snprintf(str, 100, _("Location: to be added from '%s'"), GBLvolInfo.bootRecordPathAndName);
    }
    label = gtk_label_new(str);
    gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    if(GBLvolInfo.bootRecordIsVisible)
        snprintf(str, 100, _("Is visible on image as '%s'"), GBLvolInfo.bootRecordOnImage->base.name);
    else
        snprintf(str, 100, _("Is not visible on image"));
    label = gtk_label_new(str);
    gtk_box_pack_start(GTK_BOX(vBox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    /* down here so it's centered properly over the main window */
    //gtk_widget_show(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}
