#include <stdbool.h>

void acceptFsPathCbk(GtkEntry *entry, gpointer user_data);
void buildFsBrowser(GtkWidget* boxToPackInto);
void buildFsDriveSelector(GtkWidget* boxToPackInto);
void buildFsLocator(GtkWidget* boxToPackInto);
bool changeFsDirectory(const char* newDirStr);
gboolean fsButtonPressedCbk(GtkWidget* fsView, GdkEventButton* event, gpointer user_data);
gboolean fsButtonReleasedCbk(GtkWidget* fsView, GdkEventButton* event, gpointer user_data);
void fsDriveChanged(GtkComboBox* comboBox, gpointer user_data);
void fsGoUpDirTreeCbk(GtkButton *button, gpointer data);
void fsRowDblClickCbk(GtkTreeView* treeview, GtkTreePath* path,
                      GtkTreeViewColumn* col, gpointer data);
void refreshFsView(void);
void showFsContextMenu(GtkWidget* fsView, GdkEventButton* event);
void showHiddenCbk(GtkButton *button, gpointer data);
