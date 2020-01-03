void addBootRecordFromFileCbk(GtkButton *button, gpointer bootRecordType);
void deleteBootRecordCbk(GtkButton *button, gpointer data);
void extractBootRecordCbk(GtkButton *button, gpointer data);
void setFileAsBootRecordCbk(GtkButton *button, gpointer data);
void setFileAsBootRecordRowCbk(GtkTreeModel* model, GtkTreePath* path,
                               GtkTreeIter* iterator, gpointer data);
void showBootInfoCbk(GtkButton *button, gpointer data);
