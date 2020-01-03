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
* David Johnson
* - open an iso file based on command-line parameter
* - print a help message when --help parameter given
******************************************************************************/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "isomaster.h"

GtkWidget* GBLmainWindow;
/* to be able to resize the two file browsers */
GtkWidget* GBLbrowserPaned;

extern AppSettings GBLappSettings;

int main(int argc, char** argv)
{
    GdkPixbuf* appIcon;
    GtkWidget* mainVBox;
    GtkWidget* mainFrame; /* to put a border around the window contents */
    GtkWidget* topPanedBox; /* to pack the top part of GBLbrowserPaned */
    GtkWidget* bottomPanedBox; /* to pack the bottom part of GBLbrowserPaned */
    GtkWidget* statusBar;

    /* if --help passed, return usage help and quit */
    if (argv[1] != NULL)
    {
        if(strcmp(argv[1], "--help") == 0)
        {
            printf("Usage: isomaster [image.iso]\n");
            return 0;
        }
    }
    
#ifdef ENABLE_NLS
    /* initialize gettext */
    bindtextdomain("isomaster", LOCALEDIR);
    bind_textdomain_codeset("isomaster", "UTF-8"); /* so that gettext() returns UTF-8 strings */
    textdomain("isomaster");
#endif
    
    gtk_init(&argc, &argv);
    
    findHomeDir();
    
    loadSettings();
    
    loadAppIcon(&appIcon);
    
    loadIcons();
    
    /* set up the signal handler for exiting editors and viewers */
    signal(SIGUSR1, sigusr1);
    signal(SIGUSR2, sigusr2);
    
    /* make sure childrent don't become zombies */
    signal(SIGCHLD, SIG_IGN);
    
    /* main window */
    GBLmainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(GBLmainWindow), 
                                GBLappSettings.windowWidth, GBLappSettings.windowHeight);
    gtk_window_set_title(GTK_WINDOW(GBLmainWindow), "ISO Master");
    gtk_window_set_icon(GTK_WINDOW(GBLmainWindow), appIcon); /* NULL is ok */
    gtk_widget_show(GBLmainWindow);
    g_signal_connect(G_OBJECT(GBLmainWindow), "delete_event",
                     G_CALLBACK(closeMainWindowCbk), NULL);
    
    mainVBox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(GBLmainWindow), mainVBox);
    gtk_widget_show(mainVBox);
    
    buildMenu(mainVBox);
    
    buildMainToolbar(mainVBox);
    
    buildFsLocator(mainVBox);
    
    mainFrame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(mainFrame), GTK_SHADOW_IN);
    gtk_box_pack_start(GTK_BOX(mainVBox), mainFrame, TRUE, TRUE, 0);
    gtk_widget_show(mainFrame);
    
    GBLbrowserPaned = gtk_vpaned_new();
    gtk_container_add(GTK_CONTAINER(mainFrame), GBLbrowserPaned);
    gtk_widget_show(GBLbrowserPaned);
    gtk_paned_set_position(GTK_PANED(GBLbrowserPaned), GBLappSettings.topPaneHeight);
    
    topPanedBox = gtk_vbox_new(FALSE, 0);
    gtk_paned_pack1(GTK_PANED(GBLbrowserPaned), topPanedBox, TRUE, FALSE);
    gtk_widget_show(topPanedBox);
    
    buildFsBrowser(topPanedBox);
    
    bottomPanedBox = gtk_vbox_new(FALSE, 0);
    gtk_paned_pack2(GTK_PANED(GBLbrowserPaned), bottomPanedBox, TRUE, FALSE);
    gtk_widget_show(bottomPanedBox);
    
    buildMiddleToolbar(bottomPanedBox);
    
    buildIsoLocator(bottomPanedBox);
    
    buildIsoBrowser(bottomPanedBox);
    
    statusBar = gtk_statusbar_new();
    gtk_widget_show(statusBar);
    gtk_box_pack_start(GTK_BOX(mainVBox), statusBar, FALSE, FALSE, 0);
    
    if(argv[1] != NULL)
        openIso(argv[1]);
    
    srandom((int)time(NULL));
    
    gtk_main();
    
    return 0;
}
