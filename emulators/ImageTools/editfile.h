void addToTempFilesList(const char* pathAndName);
gboolean checkEditFailed(gpointer data);
gboolean checkViewFailed(gpointer data);
void deleteTempFiles(void);
void editSelectedBtnCbk(GtkMenuItem *menuitem, gpointer data);
void editSelectedFsRowCbk(GtkTreeModel* model, GtkTreePath* path,
                          GtkTreeIter* iterator, gpointer data);
void editSelectedIsoRowCbk(GtkTreeModel* model, GtkTreePath* path,
                           GtkTreeIter* iterator, gpointer data);
void editSelectedBtnCbk(GtkMenuItem *menuitem, gpointer data);
char* makeRandomFilename(const char* sourceName);
void viewSelectedBtnCbk(GtkMenuItem *menuitem, gpointer data);
void viewSelectedFsRowCbk(GtkTreeModel* model, GtkTreePath* path,
                           GtkTreeIter* iterator, gpointer data);
void viewSelectedIsoRowCbk(GtkTreeModel* model, GtkTreePath* path,
                           GtkTreeIter* iterator, gpointer data);
void sigusr1(int signum);
void sigusr2(int signum);

/******************************************************************************
* FileCreated
* Linked list node, each having the path and name of a temporary file that 
* ISO Master created (for example while editing a file). */
typedef struct TempFileCreated
{
    char* pathAndName;
    
    struct TempFileCreated* next;
    
} TempFileCreated;
