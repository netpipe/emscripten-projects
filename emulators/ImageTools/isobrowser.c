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
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <libintl.h>
#include <errno.h>
#include <regex.h>

#include "isomaster.h"

extern GtkWidget* GBLmainWindow;
extern GtkWidget* GBLisoTreeView;
extern GtkListStore* GBLisoListStore;
extern char* GBLisoCurrentDir;
extern GtkWidget* GBLfsTreeView;
extern GtkListStore* GBLfsListStore;
extern char* GBLfsCurrentDir;
extern GtkWidget* GBLisoSizeLbl;
extern GtkWidget* GBLisoCurrentDirField;
extern GdkPixbuf* GBLdirPixbuf;
extern GdkPixbuf* GBLfilePixbuf;
extern AppSettings GBLappSettings;
extern GtkWidget* GBLrecentlyOpenWidgets[5];

/* info about the image being worked on */
VolInfo GBLvolInfo;
/* to know whether am working on an image */
bool GBLisoPaneActive = false;
/* to know whether any changes to the image have been requested */
bool GBLisoChangesProbable = false;
/* the size of the iso if it were written right now */
static bk_off_t GBLisoSize = 0;
/* the progress bar from the writing dialog box */
static GtkWidget* GBLWritingProgressBar;
/* the progress bar from the extracting dialog box */
static GtkWidget* GBLactivityProgressBar;
/* the column for the filename in the iso pane */
static GtkTreeViewColumn* GBLfilenameIsoColumn;
/* the window with the progress bar for writing */
GtkWidget* GBLwritingProgressWindow;
#ifdef ENABLE_SAVE_OVERWRITE
static char* openIsoPathAndName = NULL;
#endif
/* to really stop an operation, not just for one row */
static bool GBLoperationCanceled;

void activityProgressUpdaterCbk(VolInfo* volInfo)
{
    if(GBLactivityProgressBar != NULL)
    {
        gtk_progress_bar_pulse(GTK_PROGRESS_BAR(GBLactivityProgressBar)); 
    
        /* redraw progress bar */
        while(gtk_events_pending())
            gtk_main_iteration();
    }
}

gboolean activityProgressWindowDeleteCbk(GtkWidget* widget, GdkEvent* event,
                                          gpointer user_data)
{
    /* don't allow closing */
    return TRUE;
}

void acceptIsoPathCbk(GtkEntry *entry, gpointer user_data)
{
    const char* newPath;
    char* newPathTerminated;
    
    newPath = gtk_entry_get_text(entry);
    
    if(newPath[strlen(newPath) - 1] == '/')
    {
        changeIsoDirectory((char*)newPath);
    }
    else
    {
        newPathTerminated = malloc(strlen(newPath) + 2);
        if(newPathTerminated == NULL)
            fatalError("newPathTerminated = malloc(strlen(newPath) + 2) failed");
        
        strcpy(newPathTerminated, newPath);
        strcat(newPathTerminated, "/");
        
        changeIsoDirectory(newPathTerminated);
        
        free(newPathTerminated);
    }
}

void addToIsoCbk(GtkButton *button, gpointer data)
{
    GtkTreeSelection* selection;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLfsTreeView));
    
    /* CREATE and show progress bar */
    GtkWidget* progressWindow;
    GtkWidget* label;
    
    /* dialog window for the progress bar */
    progressWindow = gtk_dialog_new();
    gtk_dialog_set_has_separator(GTK_DIALOG(progressWindow), FALSE);
    gtk_window_set_modal(GTK_WINDOW(progressWindow), TRUE);
    gtk_window_set_title(GTK_WINDOW(progressWindow), _("Progress"));
    gtk_window_set_transient_for(GTK_WINDOW(progressWindow), GTK_WINDOW(GBLmainWindow));
    g_signal_connect_swapped(progressWindow, "delete-event",
                             G_CALLBACK(activityProgressWindowDeleteCbk), NULL);
    g_signal_connect_swapped(progressWindow, "response", 
                             G_CALLBACK(cancelOperation), NULL);
    
    label = gtk_label_new(_("Please wait while I'm adding the selected items..."));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    if(GBLappSettings.scanForDuplicateFiles)
    {
        label = gtk_label_new(_("(scanning for duplicate files)"));
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), label, TRUE, TRUE, 0);
        gtk_widget_show(label);
    }
    
    /* the progress bar */
    GBLactivityProgressBar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), GBLactivityProgressBar, TRUE, TRUE, 0);
    gtk_widget_show(GBLactivityProgressBar);
    
    /* button to cancel adding */
    gtk_dialog_add_button(GTK_DIALOG(progressWindow), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
    
    /* if i show it before i add the children, the window ends up being not centered */
    gtk_widget_show(progressWindow);
    /* END CREATE and show progress bar */
    
    GBLoperationCanceled = false;
    
    gtk_tree_selection_selected_foreach(selection, addToIsoEachRowCbk, NULL);
    
    gtk_widget_destroy(progressWindow);
    GBLactivityProgressBar = NULL;
    
    if(gtk_tree_selection_count_selected_rows(selection) > 0)
    /* reload iso view */
    {
        refreshIsoView();
    }
    
    /* iso size label */
    char sizeStr[20];
    GBLisoSize = 35845;
    //if(GBLvolInfo.filenameTypes & FNTYPE_JOLIET)
        GBLisoSize += 2048;
    GBLisoSize += bk_estimate_iso_size(&GBLvolInfo, FNTYPE_9660 | FNTYPE_JOLIET | FNTYPE_ROCKRIDGE);
    formatSize(GBLisoSize, sizeStr, sizeof(sizeStr));
    gtk_label_set_text(GTK_LABEL(GBLisoSizeLbl), sizeStr);
}

void addToIsoEachRowCbk(GtkTreeModel* model, GtkTreePath* path,
                        GtkTreeIter* iterator, gpointer data)
{
    int fileType;
    char* itemName;
    char* fullItemName; /* with full path */
    int rc;
    GtkWidget* warningDialog;
    
    if(GBLoperationCanceled)
        return;
    
    gtk_tree_model_get(model, iterator, COLUMN_HIDDEN_TYPE, &fileType, 
                                        COLUMN_FILENAME, &itemName, -1);
    
    if(fileType == FILE_TYPE_DIRECTORY || fileType == FILE_TYPE_REGULAR ||
       fileType == FILE_TYPE_SYMLINK)
    {
        fullItemName = (char*)malloc(strlen(GBLfsCurrentDir) + strlen(itemName) + 1);
        if(fullItemName == NULL)
            fatalError("addToIsoEachRowCbk(): malloc("
                       "strlen(GBLfsCurrentDir) + strlen(itemName) + 1) failed");
        
        strcpy(fullItemName, GBLfsCurrentDir);
        strcat(fullItemName, itemName);
        
        rc = bk_add(&GBLvolInfo, fullItemName, GBLisoCurrentDir, activityProgressUpdaterCbk);
        if(rc <= 0 && rc != BKWARNING_OPER_PARTLY_FAILED)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Failed to add '%s': '%s'"),
                                                   fullItemName,
                                                   bk_get_error_string(rc));
            gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        else
            GBLisoChangesProbable = true;
        
        free(fullItemName);
    }
    else
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "GUI error, adding anything other than "
                                               "files and directories doesn't work");
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
    }
    
    g_free(itemName);
}

bool askForPermissions(const char* fullItemName, mode_t* permissions)
{
    GtkWidget* dialog;
    GtkWidget* table;
    GtkWidget* label;
    int rc;
    int count;
    bool didChange = false;
    
    rc = bk_get_permissions(&GBLvolInfo, fullItemName, permissions);
    if(rc <= 0)
    {
        dialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_MESSAGE_ERROR,
                                        GTK_BUTTONS_CLOSE,
                                        "bk_get_permissions() failed (%s), please report bug",
                                        bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return false;
    }
    
    dialog = gtk_dialog_new_with_buttons(_("Change permissions"),
                                         GTK_WINDOW(GBLmainWindow),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    g_signal_connect(dialog, "close", G_CALLBACK(rejectDialogCbk), NULL);
    gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);
    
    table = gtk_table_new(3, 11, FALSE);
    //~ gtk_table_set_row_spacings(GTK_TABLE(table), 5);
    //~ gtk_table_set_col_spacings(GTK_TABLE(table), 5);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), table, TRUE, TRUE, 0);
    gtk_widget_show(table);
    
    label = gtk_label_new("User");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 3, 0, 1);
    gtk_widget_show(label);
    
    label = gtk_label_new("Group");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 4, 7, 0, 1);
    gtk_widget_show(label);
    
    label = gtk_label_new("Others");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 8, 11, 0, 1);
    gtk_widget_show(label);
    
    for(count = 0; count < 11; count++)
    {
        if(count == 0 || count == 4 || count == 8)
        {
            label = gtk_label_new("r");
            gtk_table_attach_defaults(GTK_TABLE(table), label, count, count + 1, 1, 2);
            gtk_widget_show(label);
        }
        else if(count == 1 || count == 5 || count == 9)
        {
            label = gtk_label_new("w");
            gtk_table_attach_defaults(GTK_TABLE(table), label, count, count + 1, 1, 2);
            gtk_widget_show(label);
        }
        else if(count == 2 || count == 6 || count == 10)
        {
            label = gtk_label_new("x");
            gtk_table_attach_defaults(GTK_TABLE(table), label, count, count + 1, 1, 2);
            gtk_widget_show(label);
        }
        else
        {
            label = gtk_label_new("-");
            gtk_table_attach_defaults(GTK_TABLE(table), label, count, count + 1, 1, 2);
            gtk_widget_show(label);
        }
    }
    
    /* CREATE checkboxes for permissions */
    GtkWidget* urCbx; /* user read */
    GtkWidget* uwCbx;
    GtkWidget* uxCbx;
    GtkWidget* grCbx; /* group read */
    GtkWidget* gwCbx;
    GtkWidget* gxCbx;
    GtkWidget* orCbx; /* others readd */
    GtkWidget* owCbx;
    GtkWidget* oxCbx;
    
    urCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), urCbx, 0, 1, 2, 3);
    if(*permissions & 0400)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(urCbx), TRUE);
    gtk_widget_show(urCbx);
    
    uwCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), uwCbx, 1, 2, 2, 3);
    if(*permissions & 0200)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(uwCbx), TRUE);
    gtk_widget_show(uwCbx);
    
    uxCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), uxCbx, 2, 3, 2, 3);
    if(*permissions & 0100)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(uxCbx), TRUE);
    gtk_widget_show(uxCbx);
    
    label = gtk_label_new("-");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 3, 4, 2, 3);
    gtk_widget_show(label);
    
    grCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), grCbx, 4, 5, 2, 3);
    if(*permissions & 0040)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(grCbx), TRUE);
    gtk_widget_show(grCbx);
    
    gwCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), gwCbx, 5, 6, 2, 3);
    if(*permissions & 0020)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gwCbx), TRUE);
    gtk_widget_show(gwCbx);
    
    gxCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), gxCbx, 6, 7, 2, 3);
    if(*permissions & 0010)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gxCbx), TRUE);
    gtk_widget_show(gxCbx);
    
    label = gtk_label_new("-");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 7, 8, 2, 3);
    gtk_widget_show(label);
    
    orCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), orCbx, 8, 9, 2, 3);
    if(*permissions & 0004)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(orCbx), TRUE);
    gtk_widget_show(orCbx);
    
    owCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), owCbx, 9, 10, 2, 3);
    if(*permissions & 0002)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(owCbx), TRUE);
    gtk_widget_show(owCbx);
    
    oxCbx = gtk_check_button_new();
    gtk_table_attach_defaults(GTK_TABLE(table), oxCbx, 10, 11, 2, 3);
    if(*permissions & 0001)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(oxCbx), TRUE);
    gtk_widget_show(oxCbx);
    /* END CREATE checkboxes for permissions */
    
    gtk_widget_show(dialog);
    
    rc = gtk_dialog_run(GTK_DIALOG(dialog));
    if(rc == GTK_RESPONSE_ACCEPT)
    {
        *permissions = 0;
        
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(urCbx)))
            *permissions |= 0400;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(uwCbx)))
            *permissions |= 0200;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(uxCbx)))
            *permissions |= 0100;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(grCbx)))
            *permissions |= 0040;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gwCbx)))
            *permissions |= 0020;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gxCbx)))
            *permissions |= 0010;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(orCbx)))
            *permissions |= 0004;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(owCbx)))
            *permissions |= 0002;
        if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(oxCbx)))
            *permissions |= 0001;
        
        didChange = true;
    }
    
    gtk_widget_destroy(dialog);
    
    return didChange;
}

void buildIsoBrowser(GtkWidget* boxToPackInto)
{
    GtkWidget* scrolledWindow;
    GtkTreeSelection *selection;
    GtkCellRenderer* renderer;
    GtkTreeViewColumn* column;
    
    GBLisoListStore = gtk_list_store_new(NUM_COLUMNS, 
                                         GDK_TYPE_PIXBUF, /* icon */
                                         G_TYPE_STRING, /* name */
                                         G_TYPE_UINT, /* size (64-bit sizes not allowed on an iso) */
                                         G_TYPE_UINT /* file type */
                                         );
    
    scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),
				   GTK_POLICY_AUTOMATIC,
				   GTK_POLICY_AUTOMATIC);
    //~ gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledWindow), 
                                        //~ GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(boxToPackInto), scrolledWindow, TRUE, TRUE, 0);
    gtk_widget_show(scrolledWindow);
    
    /* view widget */
    GBLisoTreeView = gtk_tree_view_new_with_model(GTK_TREE_MODEL(GBLisoListStore));
    gtk_tree_view_set_search_column(GTK_TREE_VIEW(GBLisoTreeView), COLUMN_FILENAME);
    g_object_unref(GBLisoListStore); /* destroy model automatically with view */
    gtk_container_add(GTK_CONTAINER(scrolledWindow), GBLisoTreeView);
    g_signal_connect(GBLisoTreeView, "row-activated", (GCallback)isoRowDblClickCbk, NULL);
    g_signal_connect(GBLisoTreeView, "select-cursor-parent", (GCallback)isoGoUpDirTreeCbk, NULL);
    g_signal_connect(GBLisoTreeView, "key-press-event", (GCallback)isoKeyPressedCbk, NULL);
    /* The problem with this is that i get a popup menu before the row is selected.
    * if i do a connect_after the handler never gets called. So no right-click menu. */
    g_signal_connect(GBLisoTreeView, "button-press-event", (GCallback)isoButtonPressedCbk, NULL);
    g_signal_connect(GBLisoTreeView, "button-release-event", (GCallback)isoButtonReleasedCbk, NULL);
    gtk_widget_show(GBLisoTreeView);
    
    /* this won't be enabled until gtk allows me to drag a multiple selection */
    //~ GtkTargetEntry targetEntry;
    //~ targetEntry.target = "text/plain";
    //~ targetEntry.flags = 0;
    //~ targetEntry.info = 0;
    //~ gtk_tree_view_enable_model_drag_dest(GTK_TREE_VIEW(GBLisoTreeView), &targetEntry, 1, GDK_ACTION_COPY);
    
    /* enable multi-line selection */
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
    
    /* filename column */
    GBLfilenameIsoColumn = gtk_tree_view_column_new();
    gtk_tree_view_column_set_title(GBLfilenameIsoColumn, _("Name"));
    gtk_tree_view_column_set_resizable(GBLfilenameIsoColumn, TRUE);
    
    renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(GBLfilenameIsoColumn, renderer, FALSE);
    gtk_tree_view_column_add_attribute(GBLfilenameIsoColumn, renderer, "pixbuf", COLUMN_ICON);
    
    renderer = gtk_cell_renderer_text_new();
    /* this would fuck up usability beyond what my patience can handle */
    //~ g_object_set(renderer, "editable", TRUE, NULL);
    gtk_tree_view_column_pack_start(GBLfilenameIsoColumn, renderer, TRUE);
    gtk_tree_view_column_add_attribute(GBLfilenameIsoColumn, renderer, "text", COLUMN_FILENAME);
    
    gtk_tree_view_column_set_sort_column_id(GBLfilenameIsoColumn, COLUMN_FILENAME);
    gtk_tree_view_column_set_expand(GBLfilenameIsoColumn, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(GBLisoTreeView), GBLfilenameIsoColumn);
    
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(GBLisoListStore), COLUMN_FILENAME, 
                                    sortByName, NULL, NULL);
    
    /* size column */
    column = gtk_tree_view_column_new();
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_set_title(column, _("Size"));
    gtk_tree_view_column_pack_start(column, renderer, FALSE);
    gtk_tree_view_column_add_attribute(column, renderer, "text", COLUMN_SIZE);
    gtk_tree_view_column_set_cell_data_func(column, renderer, sizeCellDataFunc32, NULL, NULL);
    gtk_tree_view_column_set_sort_column_id(column, COLUMN_SIZE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(GBLisoTreeView), column);
    
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(GBLisoListStore), COLUMN_SIZE, 
                                    sortBySize, NULL, NULL);
    
    /* set default sort */
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(GBLisoListStore),
                                         GBLappSettings.isoSortColumnId, 
                                         GBLappSettings.isoSortDirection);

    gtk_widget_set_sensitive(GBLisoCurrentDirField, FALSE);
    gtk_widget_set_sensitive(GBLisoTreeView, FALSE);
}

void buildIsoLocator(GtkWidget* boxToPackInto)
{
    GBLisoCurrentDirField = gtk_entry_new();
    //gtk_entry_set_editable(GTK_ENTRY(GBLisoCurrentDirField), FALSE);
    g_signal_connect(GBLisoCurrentDirField, "activate", (GCallback)acceptIsoPathCbk, NULL);
    gtk_box_pack_start(GTK_BOX(boxToPackInto), GBLisoCurrentDirField, FALSE, FALSE, 0);
    gtk_widget_show(GBLisoCurrentDirField);
}

void cancelOperation(GtkDialog* dialog, gint arg1, gpointer user_data)
{
    bk_cancel_operation(&GBLvolInfo);
    GBLoperationCanceled = true;
}

void changeIsoDirectory(const char* newDirStr)
{
    int rc;
    BkDir* newDir;
    BkFileBase* child;
    GtkTreeIter listIterator;
    GtkTreeModel* model;
    GtkWidget* warningDialog;
    
    rc = bk_get_dir_from_string(&GBLvolInfo, newDirStr, &newDir);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to change directory: '%s'"),
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        return;
    }
    
    /* for improved performance disconnect the model from tree view before udating it */
    model = gtk_tree_view_get_model(GTK_TREE_VIEW(GBLisoTreeView));
    g_object_ref(model);
    gtk_tree_view_set_model(GTK_TREE_VIEW(GBLisoTreeView), NULL);
    
    /* this is the only way to disable sorting (for a huge performance boost) */
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(GBLfsListStore), COLUMN_FILENAME, 
                                    sortVoid, NULL, NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(GBLfsListStore), COLUMN_SIZE, 
                                    sortVoid, NULL, NULL);
    
    gtk_list_store_clear(GBLisoListStore);
    
#if GTK_MINOR_VERSION >= 8
    /* to make sure width of filename column isn't bigger than needed (need gtk 2.8) */
    gtk_tree_view_column_queue_resize(GBLfilenameIsoColumn);
#endif
    
    /* add all directories to the tree */
    child = newDir->children;
    while(child != NULL)
    {
        if(IS_DIR(child->posixFileMode))
        /* directory */
        {
            gtk_list_store_append(GBLisoListStore, &listIterator);
            gtk_list_store_set(GBLisoListStore, &listIterator, 
                               COLUMN_ICON, GBLdirPixbuf,
                               COLUMN_FILENAME, child->name, 
                               COLUMN_SIZE, 0,
                               COLUMN_HIDDEN_TYPE, FILE_TYPE_DIRECTORY,
                               -1);
        }
        else if(IS_REG_FILE(child->posixFileMode))
        {
            gtk_list_store_append(GBLisoListStore, &listIterator);
            gtk_list_store_set(GBLisoListStore, &listIterator, 
                               COLUMN_ICON, GBLfilePixbuf,
                               COLUMN_FILENAME, child->name, 
                               COLUMN_SIZE, (BK_FILE_PTR(child))->size,
                               COLUMN_HIDDEN_TYPE, FILE_TYPE_REGULAR,
                               -1);
        }
        else if(IS_SYMLINK(child->posixFileMode))
        {
            gtk_list_store_append(GBLisoListStore, &listIterator);
            gtk_list_store_set(GBLisoListStore, &listIterator, 
                               COLUMN_ICON, GBLfilePixbuf,
                               COLUMN_FILENAME, child->name, 
                               COLUMN_SIZE, 0,
                               COLUMN_HIDDEN_TYPE, FILE_TYPE_SYMLINK,
                               -1);
        }
        
        child = child->next;
    }
    
    /* reconnect the model and view now */
    gtk_tree_view_set_model(GTK_TREE_VIEW(GBLisoTreeView), model);
    g_object_unref(model);
    
    /* reenable sorting */
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(GBLfsListStore), COLUMN_FILENAME, 
                                    sortByName, NULL, NULL);
    gtk_tree_sortable_set_sort_func(GTK_TREE_SORTABLE(GBLfsListStore), COLUMN_SIZE, 
                                    sortBySize, NULL, NULL);
    
    /* set current directory string */
    if(GBLisoCurrentDir != NULL)
        free(GBLisoCurrentDir);
    GBLisoCurrentDir = (char*)malloc(strlen(newDirStr) + 1);
    if(GBLisoCurrentDir == NULL)
        fatalError("changeIsoDirectory(): malloc(strlen(newDirStr) + 1) failed");
    strcpy(GBLisoCurrentDir, newDirStr);
    
    /* update the field with the path and name */
    gtk_entry_set_text(GTK_ENTRY(GBLisoCurrentDirField), GBLisoCurrentDir);
}

void changePermissionsBtnCbk(GtkMenuItem *menuitem, gpointer data)
{
    GtkTreeSelection* selection;
    
    /* do nothing if no image open */
    if(!GBLisoPaneActive)
        return;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    
    int numSelectedRows = gtk_tree_selection_count_selected_rows(selection);
    
    if(numSelectedRows == 1)
        gtk_tree_selection_selected_foreach(selection, changePermissionsRowCbk, NULL);
    else
    {
        GtkTreeModel* model = gtk_tree_view_get_model(GTK_TREE_VIEW(GBLisoTreeView));
        GList* rowsList = gtk_tree_selection_get_selected_rows(selection, NULL);
        mode_t permissions;
        bool firstRow = true;
        bool cancelled = false;
        
        GList* row = rowsList;
        while(row != NULL && !cancelled)
        {
            GtkTreeIter iter;
            char* itemName;
            char* fullItemName;
            
            gtk_tree_model_get_iter(model, &iter, row->data);
            gtk_tree_model_get(model, &iter, COLUMN_FILENAME, &itemName, -1);
            
            fullItemName = malloc(strlen(GBLisoCurrentDir) + strlen(itemName) + 1);
            if(fullItemName == NULL)
                fatalError("changePermissionsRowCbk(): malloc("
                           "strlen(GBLisoCurrentDir) + strlen(itemName) + 1) failed (out of memory?)");
            
            strcpy(fullItemName, GBLisoCurrentDir);
            strcat(fullItemName, itemName);
            
            if(firstRow)
            {
                cancelled = !askForPermissions(fullItemName, &permissions);
                firstRow = false;
            }
            
            if(!cancelled)
            {
                bk_set_permissions(&GBLvolInfo, fullItemName, permissions);
                GBLisoChangesProbable = true;
            }
            
            g_free(itemName);
            free(fullItemName);
            row = row->next;
        }
        
        g_list_foreach(rowsList, (GFunc)gtk_tree_path_free, NULL);
        g_list_free(rowsList);
    } // multiple rows
}

void changePermissionsRowCbk(GtkTreeModel* model, GtkTreePath* path,
                             GtkTreeIter* iterator, gpointer data)
{
    char* itemName;
    char* fullItemName;
    mode_t permissions;
    
    gtk_tree_model_get(model, iterator, COLUMN_FILENAME, &itemName, -1);
    
    fullItemName = malloc(strlen(GBLisoCurrentDir) + strlen(itemName) + 1);
    if(fullItemName == NULL)
        fatalError("changePermissionsRowCbk(): malloc("
                   "strlen(GBLisoCurrentDir) + strlen(itemName) + 1) failed (out of memory?)");
    
    strcpy(fullItemName, GBLisoCurrentDir);
    strcat(fullItemName, itemName);
    
    if(askForPermissions(fullItemName, &permissions))
    {
        bk_set_permissions(&GBLvolInfo, fullItemName, permissions);
        GBLisoChangesProbable = true;
    }
    
    g_free(itemName);
    free(fullItemName);
}


void closeIso(void)
{
    if(!GBLisoPaneActive)
    /* no image open or created, nothing to do */
        return;
    
    gtk_list_store_clear(GBLisoListStore);
    
    bk_destroy_vol_info(&GBLvolInfo);
    
    GBLisoSize = 0;
    gtk_label_set_text(GTK_LABEL(GBLisoSizeLbl), "");
    
    gtk_window_set_title(GTK_WINDOW(GBLmainWindow), "ISO Master");
    
    gtk_widget_set_sensitive(GBLisoCurrentDirField, FALSE);
    gtk_widget_set_sensitive(GBLisoTreeView, FALSE);
    
    GBLisoPaneActive = false;
    
#ifdef ENABLE_SAVE_OVERWRITE
    if(openIsoPathAndName != NULL)
    {
        free(openIsoPathAndName);
        openIsoPathAndName = NULL;
    }
#endif
}

bool confirmCloseIso(void)
{
    GtkWidget* warningDialog;
    gint response;
    
    warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                           GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_QUESTION,
                                           GTK_BUTTONS_NONE,
                                           _("It seems that you have made changes to the ISO but "
                                           "haven't saved them. Are you sure you want to close it?"));
    gtk_dialog_add_buttons(GTK_DIALOG(warningDialog),
                           GTK_STOCK_GO_BACK, GTK_RESPONSE_CANCEL,
                           GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
                           NULL);
    gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(warningDialog), GTK_RESPONSE_CLOSE);
    response = gtk_dialog_run(GTK_DIALOG(warningDialog));
    gtk_widget_destroy(warningDialog);
    
    if(response == GTK_RESPONSE_CLOSE)
        return true;
    else
        return false;
}

void deleteSelectedFromIso(void)
{
    GtkTreeSelection* selection;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    
    gtk_tree_selection_selected_foreach(selection, deleteFromIsoEachRowCbk, NULL);
    
    if(gtk_tree_selection_count_selected_rows(selection) > 0)
    /* reload iso view */
    {
        refreshIsoView();
    }
    
    /* iso size label */
    char sizeStr[20];
    GBLisoSize = 35845;
    //if(GBLvolInfo.filenameTypes & FNTYPE_JOLIET)
        GBLisoSize += 2048;
    GBLisoSize += bk_estimate_iso_size(&GBLvolInfo, FNTYPE_9660 | FNTYPE_JOLIET | FNTYPE_ROCKRIDGE);
    formatSize(GBLisoSize, sizeStr, sizeof(sizeStr));
    gtk_label_set_text(GTK_LABEL(GBLisoSizeLbl), sizeStr);
}

void deleteFromIsoCbk(GtkButton *button, gpointer data)
{
    deleteSelectedFromIso();
}

void deleteFromIsoEachRowCbk(GtkTreeModel* model, GtkTreePath* path,
                             GtkTreeIter* iterator, gpointer data)
{
    int fileType;
    char* itemName;
    char* fullItemName; /* with full path */
    int rc;
    GtkWidget* warningDialog;
    
    gtk_tree_model_get(model, iterator, COLUMN_HIDDEN_TYPE, &fileType, 
                                        COLUMN_FILENAME, &itemName, -1);
    
    if(fileType == FILE_TYPE_DIRECTORY || fileType == FILE_TYPE_REGULAR || 
       fileType == FILE_TYPE_SYMLINK)
    {
        fullItemName = (char*)malloc(strlen(GBLisoCurrentDir) + strlen(itemName) + 1);
        if(fullItemName == NULL)
            fatalError("deleteFromIsoEachRowCbk(): malloc("
                       "strlen(GBLisoCurrentDir) + strlen(itemName) + 1) failed");
        
        strcpy(fullItemName, GBLisoCurrentDir);
        strcat(fullItemName, itemName);
        
        rc = bk_delete(&GBLvolInfo, fullItemName);
        if(rc <= 0)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Failed to delete '%s': '%s'"),
                                                   itemName,
                                                   bk_get_error_string(rc));
            gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        else
            GBLisoChangesProbable = true;
        
        free(fullItemName);
    }
    else
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("GUI error, deleting anything other than "
                                               "files and directories doesn't work"));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
    }
    
    g_free(itemName);
}

void extractFromIsoCbk(GtkButton *button, gpointer data)
{
    GtkTreeSelection* selection;
    GtkWidget* progressWindow = NULL;
    GtkWidget* descriptionLabel;
    GtkWidget* cancelButton;
    
    if(!GBLisoPaneActive)
    /* no iso open */
        return;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    
    if(gtk_tree_selection_count_selected_rows(selection) > 0)
    {
        /* dialog window for the progress bar */
        progressWindow = gtk_dialog_new();
        gtk_dialog_set_has_separator(GTK_DIALOG(progressWindow), FALSE);
        gtk_window_set_modal(GTK_WINDOW(progressWindow), TRUE);
        gtk_window_set_title(GTK_WINDOW(progressWindow), _("Progress"));
        gtk_window_set_transient_for(GTK_WINDOW(progressWindow), GTK_WINDOW(GBLmainWindow));
        g_signal_connect_swapped(progressWindow, "delete-event",
                                 G_CALLBACK(activityProgressWindowDeleteCbk), NULL);
        g_signal_connect_swapped(progressWindow, "response", 
                                 G_CALLBACK(cancelOperation), NULL);
        
        /* just some text */
        descriptionLabel = gtk_label_new(_("Please wait while I'm extracting the selected files..."));
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), descriptionLabel, TRUE, TRUE, 0);
        gtk_widget_show(descriptionLabel);
        
        /* the progress bar */
        GBLactivityProgressBar = gtk_progress_bar_new();
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), GBLactivityProgressBar, TRUE, TRUE, 0);
        gtk_widget_show(GBLactivityProgressBar);
        
        /* button to cancel extracting */
        cancelButton = gtk_dialog_add_button(GTK_DIALOG(progressWindow), GTK_STOCK_CANCEL, GTK_RESPONSE_NONE);
        
        /* if i show it before i add the children, the window ends up being not centered */
        gtk_widget_show(progressWindow);
        
        GBLoperationCanceled = false;
        
        gtk_tree_selection_selected_foreach(selection, extractFromIsoEachRowCbk, NULL);
        
        refreshFsView();
        
        gtk_widget_destroy(progressWindow);
    }
    
    GBLactivityProgressBar = NULL;
}

void extractFromIsoEachRowCbk(GtkTreeModel* model, GtkTreePath* path,
                              GtkTreeIter* iterator, gpointer data)
{
    int fileType;
    char* itemName;
    char* fullItemName; /* with full path */
    int rc;
    GtkWidget* warningDialog;
    
    if(GBLoperationCanceled)
        return;
    
    gtk_tree_model_get(model, iterator, COLUMN_HIDDEN_TYPE, &fileType, 
                                        COLUMN_FILENAME, &itemName, -1);
    
    fullItemName = (char*)malloc(strlen(GBLisoCurrentDir) + strlen(itemName) + 1);
    if(fullItemName == NULL)
        fatalError("extractFromIsoEachRowCbk(): malloc("
                   "strlen(GBLisoCurrentDir) + strlen(itemName) + 1) failed (out of memory?)");
    
    strcpy(fullItemName, GBLisoCurrentDir);
    strcat(fullItemName, itemName);
    
    rc = bk_extract(&GBLvolInfo, fullItemName, GBLfsCurrentDir, 
                    true, activityProgressUpdaterCbk);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to extract '%s': '%s'"),
                                               itemName,
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
    }
    
    free(fullItemName);
    
    g_free(itemName);
}

/******************************************************************************
* isoButtonPressedCbk()
* Make sure that a right-click on the view doesn't send the signal to the
* widget. If it did, a selection of multiple rows would be lost.
* I have a feeling someone did this shit in GTK just to piss on users */
gboolean isoButtonPressedCbk(GtkWidget* isoView, GdkEventButton* event, gpointer user_data)
{
    if(!GBLisoPaneActive)
    /* no iso open */
        return FALSE;
    
    if(event->type == GDK_BUTTON_PRESS  &&  event->button == 3)
    {
        /* Stop event propagation */
        /*!! Would be nice if I could only stop event propagation if click was on
        * the selection, I have to look into how that may be done, if at all */
        return TRUE;
    }
    
    return FALSE;
}

/******************************************************************************
* isoButtonReleasedCbk()
* Show context menu if releasing the right mouse button */
gboolean isoButtonReleasedCbk(GtkWidget* isoView, GdkEventButton* event, gpointer user_data)
{
    if(!GBLisoPaneActive)
    /* no iso open */
        return FALSE;
    
    if(event->type == GDK_BUTTON_RELEASE &&  event->button == 3)
    {
        showIsoContextMenu(isoView, event);
    }
    
    return FALSE;
}

/* this is called from a button and via a treeview event so don't use the parameters */
void isoGoUpDirTreeCbk(GtkButton *button, gpointer data)
{
    int count;
    bool done;
    char* newCurrentDir;
    GtkWidget* warningDialog;
    
    /* do nothing if no image open */
    if(!GBLisoPaneActive)
        return;
    
    /* do nothing if already at root */
    if(GBLisoCurrentDir[0] == '/' && GBLisoCurrentDir[1] == '\0')
        return;
    
    /* need to allocate a new string because changeIsoDirectory() uses it 
    * to copy from after freeing GBLisoCurrentDir */
    newCurrentDir = (char*)malloc(strlen(GBLisoCurrentDir) + 1);
    if(GBLisoCurrentDir == NULL)
        fatalError("isoGoUpDirTree(): malloc(strlen(GBLisoCurrentDir) + 1) failed");
    strcpy(newCurrentDir, GBLisoCurrentDir);
    
    /* look for the second last slash */
    done = false;
    for(count = strlen(newCurrentDir) - 1; !done && count >= 0; count--)
    {
        if(newCurrentDir[count - 1] == '/')
        /* truncate the string */
        {
            newCurrentDir[count] = '\0';
            changeIsoDirectory(newCurrentDir);
            done = true;
        }
    }
    
    if(!done)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "GUI error, GBLisoCurrentDir is not '/' and has "
                                               "only one slash, please report bug.");
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
    }
    
    free(newCurrentDir);
}

gboolean isoKeyPressedCbk(GtkWidget* widget, GdkEventKey* event, gpointer user_data)
{
    if(event->keyval == GDK_Delete)
    {
        deleteFromIsoCbk(NULL, NULL);
        
        return TRUE;
    }
    
    return FALSE;
}

void isoRowDblClickCbk(GtkTreeView* treeview, GtkTreePath* path,
                       GtkTreeViewColumn* col, gpointer data)
{
    GtkTreeModel* model;
    GtkTreeIter iterator;
    char* name;
    char* newCurrentDir;
    int fileType;
    GtkWidget* warningDialog;
    
    model = gtk_tree_view_get_model(treeview);
    
    if(gtk_tree_model_get_iter(model, &iterator, path) == FALSE)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "GUI error: 'isoRowDblClicked(): "
                                               "gtk_tree_model_get_iter() failed'");
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        return;
    }
    
    gtk_tree_model_get(model, &iterator, COLUMN_HIDDEN_TYPE, &fileType, -1);
    if(fileType == FILE_TYPE_DIRECTORY)
    {
        gtk_tree_model_get(model, &iterator, COLUMN_FILENAME, &name, -1);
        
        newCurrentDir = (char*)malloc(strlen(GBLisoCurrentDir) + strlen(name) + 2);
        if(newCurrentDir == NULL)
            fatalError("isoRowDblClicked(): malloc("
                       "strlen(GBLisoCurrentDir) + strlen(name) + 2) failed");
        
        strcpy(newCurrentDir, GBLisoCurrentDir);
        strcat(newCurrentDir, name);
        strcat(newCurrentDir, "/");
        
        changeIsoDirectory(newCurrentDir);
        
        free(newCurrentDir);
        g_free(name);
    }
    /* else do nothing (not a directory) */
}

/* This callback is also used by an accelerator so make sure you don't use 
* the parameters, since they may not be the menuitem parameters */
gboolean newIsoCbk(GtkMenuItem* menuItem, gpointer data)
{
    int rc;
    GtkWidget* warningDialog;
    
    if(GBLisoChangesProbable && !confirmCloseIso())
        return TRUE;
    
    closeIso();
    
    rc = bk_init_vol_info(&GBLvolInfo, GBLappSettings.scanForDuplicateFiles);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to initialise bkisofs: '%s'"),
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        return TRUE;
    }
    
    GBLvolInfo.warningCbk = operationFailed;
    
    GBLappSettings.filenameTypesToWrite = FNTYPE_9660 | FNTYPE_ROCKRIDGE | FNTYPE_JOLIET;
    
    /* iso size label */
    char sizeStr[20];
    GBLisoSize = 35845;
    //if(GBLvolInfo.filenameTypes & FNTYPE_JOLIET)
        GBLisoSize += 2048;
    GBLisoSize += bk_estimate_iso_size(&GBLvolInfo, FNTYPE_9660 | FNTYPE_JOLIET | FNTYPE_ROCKRIDGE);
    formatSize(GBLisoSize, sizeStr, sizeof(sizeStr));
    gtk_label_set_text(GTK_LABEL(GBLisoSizeLbl), sizeStr);
    
    gtk_widget_set_sensitive(GBLisoCurrentDirField, TRUE);
    gtk_widget_set_sensitive(GBLisoTreeView, TRUE);
    
    GBLisoPaneActive = true;
    
    GBLisoChangesProbable = false;
    
    changeIsoDirectory("/");
    
    /* the accelerator callback must return true */
    return TRUE;
}

void openIso(char* filename)
{
    int rc;
    GtkWidget* warningDialog;
    
    /* MAYBE move around in recently open list */
    int alreadyInListIndex = -1;
    for(int i = 0; i < 5; i++)
    {
        const char* oldText = gtk_label_get_text(GTK_LABEL(
                gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[i]))));
        if(strcmp(oldText, filename) == 0)
            alreadyInListIndex = i;
    }
    //!! flipping them is weird, what's really needed is to put the found
    // one up and push everything else down once, but that's too much work
    // so for now just add to the list if it's not already there, and do
    // nothing otherwise
    //~ if(alreadyInListIndex != -1)
    //~ {
        //~ const char* oldFirst = gtk_label_get_text(GTK_LABEL(
                //~ gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[0]))));
        
        //~ // flip the two
        //~ printf("flip '%s' '%s'\n", oldFirst, gtk_label_get_text(GTK_LABEL(
                    //~ gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[alreadyInListIndex])))));
        //~ gtk_label_set_text(GTK_LABEL(
                    //~ gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[alreadyInListIndex]))),
                    //~ oldFirst);
        //~ gtk_label_set_text(GTK_LABEL(
                    //~ gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[0]))),
                    //~ filename);
    //~ }
    /* END MAYBE move around in recently open list */
    
    /* MAYBE record recently open */
    if(alreadyInListIndex == -1)
    {
        // move all existing ones down once
        for(int i = 4; i > 0; i--)
        {
            const char* oldText = gtk_label_get_text(GTK_LABEL(
                    gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[i - 1]))));
            gtk_label_set_text(GTK_LABEL(
                    gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[i]))),
                    oldText);
            if(oldText[0] != '\0')
                gtk_widget_show(GBLrecentlyOpenWidgets[i]);
        }
        gtk_label_set_text(GTK_LABEL(
                    gtk_bin_get_child(GTK_BIN(GBLrecentlyOpenWidgets[0]))),
                    filename);
        gtk_widget_show(GBLrecentlyOpenWidgets[0]);
    }
    /* END MAYBE record recently open */
    
    if(GBLisoChangesProbable && !confirmCloseIso())
        return;
    
    closeIso();
    
    rc = bk_init_vol_info(&GBLvolInfo, GBLappSettings.scanForDuplicateFiles);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to initialise bkisofs: '%s'"),
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        return;
    }
    
    GBLvolInfo.warningCbk = operationFailed;
    
    GBLappSettings.filenameTypesToWrite = FNTYPE_9660 | FNTYPE_ROCKRIDGE | FNTYPE_JOLIET;
    
    rc = bk_open_image(&GBLvolInfo, filename);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to open iso file for reading: '%s'"),
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        return;
    }
    
    rc = bk_read_vol_info(&GBLvolInfo);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to read volume info: '%s'"),
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        closeIso();
        return;
    }
    
    /* CREATE and show progress bar */
    GtkWidget* progressWindow;
    GtkWidget* label;
    
    /* dialog window for the progress bar */
    progressWindow = gtk_dialog_new();
    gtk_dialog_set_has_separator(GTK_DIALOG(progressWindow), FALSE);
    gtk_window_set_modal(GTK_WINDOW(progressWindow), TRUE);
    gtk_window_set_title(GTK_WINDOW(progressWindow), _("Progress"));
    gtk_window_set_transient_for(GTK_WINDOW(progressWindow), GTK_WINDOW(GBLmainWindow));
    g_signal_connect_swapped(progressWindow, "delete-event",
                             G_CALLBACK(activityProgressWindowDeleteCbk), NULL);
    g_signal_connect_swapped(progressWindow, "response", 
                             G_CALLBACK(cancelOperation), NULL);
    
    label = gtk_label_new(_("Please wait while I'm reading the image..."));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), label, TRUE, TRUE, 0);
    gtk_widget_show(label);
    
    if(GBLappSettings.scanForDuplicateFiles)
    {
        label = gtk_label_new(_("(scanning for duplicate files)"));
        gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), label, TRUE, TRUE, 0);
        gtk_widget_show(label);
    }
    
    /* the progress bar */
    GBLactivityProgressBar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(progressWindow)->vbox), GBLactivityProgressBar, TRUE, TRUE, 0);
    gtk_widget_show(GBLactivityProgressBar);
    
    /* button to cancel adding */
    gtk_dialog_add_button(GTK_DIALOG(progressWindow), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
    
    /* if i show it before i add the children, the window ends up being not centered */
    gtk_widget_show(progressWindow);
    /* END CREATE and show progress bar */
    
    /* READ entire directory tree */
    if(GBLvolInfo.filenameTypes & FNTYPE_ROCKRIDGE)
        rc = bk_read_dir_tree(&GBLvolInfo, FNTYPE_ROCKRIDGE, true, activityProgressUpdaterCbk);
    else if(GBLvolInfo.filenameTypes & FNTYPE_JOLIET)
        rc = bk_read_dir_tree(&GBLvolInfo, FNTYPE_JOLIET, false, activityProgressUpdaterCbk);
    else
        rc = bk_read_dir_tree(&GBLvolInfo, FNTYPE_9660, false, activityProgressUpdaterCbk);
    if(rc <= 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to read directory tree: '%s'"),
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
    }
    /* END READ entire directory tree */
    
    gtk_widget_destroy(progressWindow);
    GBLactivityProgressBar = NULL;
    
    if(rc <= 0)
    {
        closeIso();
        return;
    }
    
#ifdef ENABLE_SAVE_OVERWRITE
    openIsoPathAndName = malloc(strlen(filename) + 1);
    strcpy(openIsoPathAndName, filename);
#endif
    
    /* iso size label */
    char sizeStr[20];
    GBLisoSize = 35845;
    //if(GBLvolInfo.filenameTypes & FNTYPE_JOLIET)
        GBLisoSize += 2048;
    
    /* SET WINDOW title */
    int filenameLen = strlen(filename);
    
    char* windowTitle = malloc(filenameLen + strlen(" - ISO Master") + 1);
    if(windowTitle == NULL)
            fatalError("windowTitle = malloc(strlen(filename) + \" - ISO Master\" + 1) failed");
    
    int lastSlashIndex = -1;
    int count;
    for(count = 0; count < filenameLen; count++)
    {
        if(filename[count] == '/')
            lastSlashIndex = count;
    }
    
    if(lastSlashIndex == -1)
        strcpy(windowTitle, filename);
    else
        strcpy(windowTitle, filename + lastSlashIndex + 1);
    strcat(windowTitle, " - ISO Master");
    
    gtk_window_set_title(GTK_WINDOW(GBLmainWindow), windowTitle);
    
    free(windowTitle);
    /* END SET WINDOW title */
    
    GBLisoSize += bk_estimate_iso_size(&GBLvolInfo, FNTYPE_9660 | FNTYPE_JOLIET | FNTYPE_ROCKRIDGE);
    formatSize(GBLisoSize, sizeStr, sizeof(sizeStr));
    gtk_label_set_text(GTK_LABEL(GBLisoSizeLbl), sizeStr);
    
    gtk_widget_set_sensitive(GBLisoCurrentDirField, TRUE);
    gtk_widget_set_sensitive(GBLisoTreeView, TRUE);
    
    changeIsoDirectory("/");
    
    GBLisoPaneActive = true;
    
    GBLisoChangesProbable = false;
}

/* This callback is also used by an accelerator so make sure you don't use 
* the parameters, since they may not be the menuitem parameters */
gboolean openIsoCbk(GtkMenuItem* menuItem, gpointer data)
{
    GtkWidget *dialog;
    char* filename = NULL;
    GtkFileFilter* nameFilter;
    int dialogRespose;
    
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                         NULL);
    
    nameFilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(nameFilter), "*.[iI][sS][oO]");
    gtk_file_filter_set_name(GTK_FILE_FILTER(nameFilter), _("ISO Images"));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(nameFilter));
    
    nameFilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(nameFilter), "*.[nN][rR][gG]");
    gtk_file_filter_set_name(GTK_FILE_FILTER(nameFilter), _("NRG Images"));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(nameFilter));
    
    nameFilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(nameFilter), "*.[mM][dD][fF]");
    gtk_file_filter_set_name(GTK_FILE_FILTER(nameFilter), _("MDF Images"));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(nameFilter));
    
    nameFilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(nameFilter), "*");
    gtk_file_filter_set_name(GTK_FILE_FILTER(nameFilter), _("All files"));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(nameFilter));
    
    if(GBLappSettings.lastIsoDir != NULL)
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), GBLappSettings.lastIsoDir);
    
    dialogRespose = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if(dialogRespose == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        /* RECORD last iso dir */
        char* lastIsoDir = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        
        if(GBLappSettings.lastIsoDir != NULL && strlen(lastIsoDir) > strlen(GBLappSettings.lastIsoDir))
        {
            free(GBLappSettings.lastIsoDir);
            GBLappSettings.lastIsoDir = NULL;
        }
        
        if(GBLappSettings.lastIsoDir == NULL)
            GBLappSettings.lastIsoDir = malloc(strlen(lastIsoDir) + 1);
        
        strcpy(GBLappSettings.lastIsoDir, lastIsoDir);
        
        g_free(lastIsoDir);
        /* END RECORD last iso dir */
    }
    
    gtk_widget_destroy(dialog);
    
    if(dialogRespose == GTK_RESPONSE_ACCEPT)
    {
        openIso(filename);
        
        g_free(filename);
    }
    
    //~ openIso("image.iso");
    
    /* the accelerator callback must return true */
    return TRUE;
}

void openRecentIso(GtkMenuItem *menuitem, gpointer data)
{
    const char* filename = gtk_label_get_text(GTK_LABEL(
                    gtk_bin_get_child(GTK_BIN(menuitem))));
    openIso((char*)filename);
}

bool operationFailed(const char* msg)
{
    GtkWidget* warningDialog;
    gint response;
    
    warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                           GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_WARNING,
                                           GTK_BUTTONS_YES_NO,
                                           _("%s\n\nDo you wish to continue?"),
                                           msg);
    gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
    gtk_dialog_set_default_response(GTK_DIALOG(warningDialog), GTK_RESPONSE_YES);
    response = gtk_dialog_run(GTK_DIALOG(warningDialog));
    gtk_widget_destroy(warningDialog);
    
    if(response == GTK_RESPONSE_YES)
        return true;
    else
    {
        GBLoperationCanceled = true;
        return false;
    }
}

void refreshIsoView(void)
{
    char* isoCurrentDir; /* for changeIsoDirectory() */
    
    isoCurrentDir = malloc(strlen(GBLisoCurrentDir) + 1);
    if(isoCurrentDir == NULL)
        fatalError("refreshIsoView(): malloc("
                   "strlen(GBLisoCurrentDir) + 1) failed");
    strcpy(isoCurrentDir, GBLisoCurrentDir);
    
    /* remember scroll position */
    GdkRectangle visibleRect;
    gtk_tree_view_get_visible_rect(GTK_TREE_VIEW(GBLisoTreeView), &visibleRect);
    
    changeIsoDirectory(isoCurrentDir);
    
    /* need the -1 because if i call this function with the same coordinates that 
    * the view already has, the position is set to 0. think it's a gtk bug. */
    gtk_tree_view_scroll_to_point(GTK_TREE_VIEW(GBLisoTreeView), visibleRect.x - 1, visibleRect.y - 1);
    
    free(isoCurrentDir);
}

void renameSelected(void)
{
    GtkTreeSelection* selection;
    
    /* do nothing if no image open */
    if(!GBLisoPaneActive)
        return;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    
    if(gtk_tree_selection_count_selected_rows(selection) != 1)
        return;
    
    /* there's just one row selected but this is the easiest way to do it */
    gtk_tree_selection_selected_foreach(selection, renameSelectedRowCbk, NULL);
    
    /* can't put this in the callback because gtk complains */
    refreshIsoView();
}

void renameSelectedRowCbk(GtkTreeModel* model, GtkTreePath* path,
                          GtkTreeIter* iterator, gpointer data)
{
    GtkWidget* dialog;
    GtkWidget* nameField;
    int rc;
    char* itemName;
    char* fullItemName;
    GtkWidget* warningDialog;
    
    gtk_tree_model_get(model, iterator, COLUMN_FILENAME, &itemName, -1);
    
    fullItemName = (char*)malloc(strlen(GBLisoCurrentDir) + strlen(itemName) + 1);
    if(fullItemName == NULL)
        fatalError("extractFromIsoEachRowCbk(): malloc("
                   "strlen(GBLisoCurrentDir) + strlen(itemName) + 1) failed (out of memory?)");
    
    strcpy(fullItemName, GBLisoCurrentDir);
    strcat(fullItemName, itemName);
    
    dialog = gtk_dialog_new_with_buttons(_("Enter a new name:"),
                                         GTK_WINDOW(GBLmainWindow),
                                         GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_STOCK_OK,
                                         GTK_RESPONSE_ACCEPT,
                                         GTK_STOCK_CANCEL,
                                         GTK_RESPONSE_REJECT,
                                         NULL);
    g_signal_connect(dialog, "close", G_CALLBACK(rejectDialogCbk), NULL);
    
    nameField = gtk_entry_new_with_max_length(NCHARS_FILE_ID_MAX_STORE);
    gtk_entry_set_text(GTK_ENTRY(nameField), itemName);
    gtk_entry_set_width_chars(GTK_ENTRY(nameField), 32);
    g_signal_connect(nameField, "activate", (GCallback)acceptDialogCbk, dialog);
    gtk_widget_show(nameField);
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), nameField, TRUE, TRUE, 0);
    
    gtk_widget_show(dialog);
    
    rc = gtk_dialog_run(GTK_DIALOG(dialog));
    if(rc == GTK_RESPONSE_ACCEPT)
    {
        rc = bk_rename(&GBLvolInfo, fullItemName, gtk_entry_get_text(GTK_ENTRY(nameField)));
        if(rc < 0)
        {
            warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                                   GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_ERROR,
                                                   GTK_BUTTONS_CLOSE,
                                                   _("Failed to rename '%s': %s"),
                                                   itemName,
                                                   bk_get_error_string(rc));
            gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
            gtk_dialog_run(GTK_DIALOG(warningDialog));
            gtk_widget_destroy(warningDialog);
        }
        else
            GBLisoChangesProbable = true;
    }
    
    gtk_widget_destroy(dialog);
    
    g_free(itemName);
    free(fullItemName);
}

void renameSelectedBtnCbk(GtkMenuItem *menuitem, gpointer data)
{
    /* because I'm lazy just call this one, it will work */
    renameSelected();
}

void saveIso(char* filename)
{
    int rc;
    GtkWidget* descriptionLabel;
    GtkWidget* okButton;
    GtkWidget* cancelButton;
    GtkWidget* warningDialog;
    
    /* dialog window for the progress bar */
    GBLwritingProgressWindow = gtk_dialog_new();
    gtk_dialog_set_has_separator(GTK_DIALOG(GBLwritingProgressWindow), FALSE);
    gtk_window_set_modal(GTK_WINDOW(GBLwritingProgressWindow), TRUE);
    gtk_window_set_title(GTK_WINDOW(GBLwritingProgressWindow), _("Progress"));
    gtk_window_set_transient_for(GTK_WINDOW(GBLwritingProgressWindow), GTK_WINDOW(GBLmainWindow));
    g_signal_connect_swapped(GBLwritingProgressWindow, "delete-event",
                             G_CALLBACK(writingProgressWindowDeleteCbk), NULL);
    g_signal_connect_swapped(GBLwritingProgressWindow, "response",
                             G_CALLBACK(writingProgressResponse), GBLwritingProgressWindow);
    g_signal_connect_swapped(GBLwritingProgressWindow, "destroy",
                             G_CALLBACK(writingProgressWindowDestroyedCbk), NULL);
    
    /* just some text */
    descriptionLabel = gtk_label_new(_("Please wait while I'm saving the new image to disk..."));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(GBLwritingProgressWindow)->vbox), descriptionLabel, TRUE, TRUE, 0);
    gtk_widget_show(descriptionLabel);
    
    /* the progress bar */
    GBLWritingProgressBar = gtk_progress_bar_new();
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(GBLwritingProgressWindow)->vbox), GBLWritingProgressBar, TRUE, TRUE, 0);
    gtk_widget_show(GBLWritingProgressBar);
    
    /* button to close the dialog (disabled until writing finished) */
    okButton = gtk_dialog_add_button(GTK_DIALOG(GBLwritingProgressWindow), GTK_STOCK_OK, GTK_RESPONSE_OK);
    gtk_widget_set_sensitive(okButton, FALSE);
    
    /* button to cancel writing */
    cancelButton = gtk_dialog_add_button(GTK_DIALOG(GBLwritingProgressWindow), GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL);
    
    /* if i show it before i add the children, the window ends up being not centered */
    gtk_widget_show(GBLwritingProgressWindow);
    
    /* write new image */
    rc = bk_write_image(filename, &GBLvolInfo, time(NULL), GBLappSettings.filenameTypesToWrite, 
                        writingProgressUpdaterCbk);
    if(rc < 0)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               _("Failed to write image to '%s': '%s'"),
                                               filename,
                                               bk_get_error_string(rc));
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        if(GBLWritingProgressBar != NULL)
            gtk_widget_destroy(GBLwritingProgressWindow);
    }
    else
        GBLisoChangesProbable = false;
    
    if(GBLWritingProgressBar != NULL)
    /* progress window hasn't been destroyed */
    {
        /* enable the ok button so the user can close the progress window */
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GBLWritingProgressBar), 1.0);
        gtk_widget_set_sensitive(okButton, TRUE);
        gtk_widget_grab_focus(okButton);
        gtk_widget_set_sensitive(cancelButton, FALSE);
    }
}

/* This callback is also used by an accelerator so make sure you don't use 
* the parameters, since they may not be the menuitem parameters */
gboolean saveIsoCbk(GtkWidget *widget, GdkEvent *event)
{
    GtkWidget *dialog;
    char* filename = NULL;
    int dialogResponse;
    GtkFileFilter* nameFilter;
    GtkWidget* addExtensionCheckbox;
    bool askedToAddExtension;
    
    /* do nothing if no image open */
    if(!GBLisoPaneActive)
        return TRUE;
    
    dialog = gtk_file_chooser_dialog_new(_("Save File"),
                                         NULL,
                                         GTK_FILE_CHOOSER_ACTION_SAVE,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                         GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
                                         NULL);
    // gtk problem? enter doesn't work
    //g_signal_connect(dialog, "key-press-event", (GCallback), NULL);
    
    nameFilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(nameFilter), "*.[iI][sS][oO]");
    gtk_file_filter_set_name(GTK_FILE_FILTER(nameFilter), _("ISO Images"));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(nameFilter));
    
    nameFilter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(nameFilter), "*");
    gtk_file_filter_set_name(GTK_FILE_FILTER(nameFilter), _("All files"));
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(nameFilter));
    
    addExtensionCheckbox = gtk_check_button_new_with_label(_("Add extension automatically"));
    gtk_file_chooser_set_extra_widget(GTK_FILE_CHOOSER(dialog), addExtensionCheckbox);
    if(GBLappSettings.appendExtension)
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(addExtensionCheckbox), TRUE);
    else
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(addExtensionCheckbox), FALSE);
    gtk_widget_show(addExtensionCheckbox);
    
    if(GBLappSettings.lastIsoDir != NULL)
        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), GBLappSettings.lastIsoDir);
    
    dialogResponse = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if(dialogResponse == GTK_RESPONSE_ACCEPT)
    {
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        /* RECORD last iso dir */
        char* lastIsoDir = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dialog));
        
        if(GBLappSettings.lastIsoDir != NULL && strlen(lastIsoDir) > strlen(GBLappSettings.lastIsoDir))
        {
            free(GBLappSettings.lastIsoDir);
            GBLappSettings.lastIsoDir = NULL;
        }
        
        if(GBLappSettings.lastIsoDir == NULL)
            GBLappSettings.lastIsoDir = malloc(strlen(lastIsoDir) + 1);
        
        strcpy(GBLappSettings.lastIsoDir, lastIsoDir);
        
        g_free(lastIsoDir);
        /* END RECORD iso save dir */
        
        askedToAddExtension = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(addExtensionCheckbox));
    }
    
    gtk_widget_destroy(dialog);
    
    if(dialogResponse == GTK_RESPONSE_ACCEPT)
    {
        char* nameWithExtension;
        
        nameWithExtension = malloc(strlen(filename) + 5);
        if(nameWithExtension == NULL)
            fatalError("saveIsoCbk(): malloc(strlen(filename) + 5) failed");
        
        strcpy(nameWithExtension, filename);
        g_free(filename);
        
        if(askedToAddExtension)
        {
            regex_t extensionRegex;
            
            regcomp(&extensionRegex, ".*\\.[iI][sS][oO]$", 0);
            
            if(regexec(&extensionRegex, nameWithExtension, 0, NULL, 0) != 0)
            /* doesn't already end with .iso */
                strcat(nameWithExtension, ".iso");
            
            GBLappSettings.appendExtension = true;
        }
        else
            GBLappSettings.appendExtension = false;
        
        saveIso(nameWithExtension);
        
        free(nameWithExtension);
    }
    
    //~ saveIso("/home/andrei/out.iso");
    
    /* the accelerator callback must return true */
    return TRUE;
}

#ifdef ENABLE_SAVE_OVERWRITE
#define TEMPFILENAME "/tmp/isomaster-temp.iso"
gboolean saveOverwriteIsoCbk(GtkWidget *widget, GdkEvent *event)
{
    int sourceFile;
    int destFile;
    int numBytesRead;
    char line[1024];
    GtkWidget* warningDialog;
    
    saveIso(TEMPFILENAME);
    
    printf("moving %s to %s\n", TEMPFILENAME, openIsoPathAndName);
    
    sourceFile = open(TEMPFILENAME, O_RDONLY);
    if(sourceFile == -1)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "Failed to open %s for reading",
                                               TEMPFILENAME);
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        if(GBLWritingProgressBar != NULL)
            gtk_widget_destroy(GBLwritingProgressWindow);
    }
    
    destFile = open(openIsoPathAndName, O_WRONLY | O_CREAT);
    if(destFile == -1)
    {
        warningDialog = gtk_message_dialog_new(GTK_WINDOW(GBLmainWindow),
                                               GTK_DIALOG_DESTROY_WITH_PARENT,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_CLOSE,
                                               "Failed to open %s for writing",
                                               openIsoPathAndName);
        gtk_window_set_modal(GTK_WINDOW(warningDialog), TRUE);
        gtk_dialog_run(GTK_DIALOG(warningDialog));
        gtk_widget_destroy(warningDialog);
        if(GBLWritingProgressBar != NULL)
            gtk_widget_destroy(GBLwritingProgressWindow);
    }
    
    while((numBytesRead = read(sourceFile, line, sizeof(line))) > 0)
      write(destFile, line, numBytesRead);
    
    close(sourceFile);
    close(destFile);
    
    return FALSE;
}
#endif

void showIsoContextMenu(GtkWidget* isoView, GdkEventButton* event)
{
    GtkWidget* menu;
    GtkWidget* menuItem;
    GtkTreeSelection* selection;
    gint numSelectedRows;
    GtkAccelGroup* accelGroup;
    
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(GBLisoTreeView));
    
    numSelectedRows = gtk_tree_selection_count_selected_rows(selection);
    if(numSelectedRows == 0)
        return;
    
    /* have this here just so that the shortcut keys show in the context menu */
    accelGroup = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(GBLmainWindow), accelGroup);
    
    menu = gtk_menu_new();
    gtk_menu_set_accel_group(GTK_MENU(menu), accelGroup);
    
    menuItem = gtk_image_menu_item_new_with_label(_("Rename"));
    g_signal_connect(menuItem, "activate", 
                     (GCallback)renameSelectedBtnCbk, NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/Contextmenu/Rename");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show_all(menu);
    if(numSelectedRows > 1)
        gtk_widget_set_sensitive(menuItem, FALSE);
    
    menuItem = gtk_image_menu_item_new_with_label(_("View"));
    g_signal_connect(menuItem, "activate", 
                     (GCallback)viewSelectedBtnCbk, NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/Contextmenu/View");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show_all(menu);
    
    menuItem = gtk_image_menu_item_new_with_label(_("Edit"));
    g_signal_connect(menuItem, "activate", 
                     (GCallback)editSelectedBtnCbk, NULL);
    gtk_menu_item_set_accel_path(GTK_MENU_ITEM(menuItem), "<ISOMaster>/Contextmenu/Edit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show_all(menu);
    
    menuItem = gtk_image_menu_item_new_with_label(_("Change permissions"));
    g_signal_connect(menuItem, "activate", 
                     (GCallback)changePermissionsBtnCbk, NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuItem);
    gtk_widget_show_all(menu);
    //~ if(numSelectedRows > 1)
        //~ gtk_widget_set_sensitive(menuItem, FALSE);
    
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                   event->button, gdk_event_get_time((GdkEvent*)event));
}

/* this handles the ok and cancel buttons in the progress window */
void writingProgressResponse(GtkDialog* dialog, gint arg1, gpointer user_data)
{
    if(arg1 == GTK_RESPONSE_CANCEL)
        bk_cancel_operation(&GBLvolInfo);
    else if(arg1 == GTK_RESPONSE_OK)
        gtk_widget_destroy(GBLwritingProgressWindow);
}

void writingProgressUpdaterCbk(VolInfo* volInfo, double percentComplete)
{
    if(GBLWritingProgressBar != NULL)
    {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(GBLWritingProgressBar), percentComplete / 100);
        
        /* redraw progress bar */
        while(gtk_events_pending())
            gtk_main_iteration();
    }
}

gboolean writingProgressWindowDeleteCbk(GtkWidget* widget, GdkEvent* event,
                                        gpointer user_data)
{
    /* don't allow closing */
    return TRUE;
}

void writingProgressWindowDestroyedCbk(void)
{
    GBLwritingProgressWindow = NULL;
    GBLWritingProgressBar = NULL;
}
