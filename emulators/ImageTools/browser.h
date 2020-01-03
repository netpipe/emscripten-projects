enum
{
    COLUMN_ICON = 0,
    COLUMN_FILENAME,
    COLUMN_SIZE,
    COLUMN_HIDDEN_TYPE,
    NUM_COLUMNS
};

enum
{
    FILE_TYPE_REGULAR = 0,
    FILE_TYPE_DIRECTORY,
    FILE_TYPE_SYMLINK
};

void acceptDialogCbk(GtkEntry *entry, GtkDialog* dialog);
void createDirCbk(GtkButton *button, gpointer onFs);
void formatSize(bk_off_t sizeInt, char* sizeStr, int sizeStrLen);
void refreshBothViewsCbk(GtkWidget *widget, GdkEvent *event);
void sizeCellDataFunc32(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                        GtkTreeModel *model, GtkTreeIter *iter,
                        gpointer data);
void sizeCellDataFunc64(GtkTreeViewColumn *col, GtkCellRenderer *renderer,
                        GtkTreeModel *model, GtkTreeIter *iter,
                        gpointer data);
gint sortByName(GtkTreeModel *model, GtkTreeIter *a, 
                GtkTreeIter *b, gpointer userdata);
gint sortBySize(GtkTreeModel *model, GtkTreeIter *a, 
                GtkTreeIter *b, gpointer userdata);
gint sortVoid(GtkTreeModel *model, GtkTreeIter *a, 
              GtkTreeIter *b, gpointer userdata);
