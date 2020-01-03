#ifndef bkPath_h
#define bkPath_h

#include "bkInternal.h"

bool findDirByNewPath(const NewPath* path, BkDir* tree, BkDir** dir);
bool findBaseByNewPath(NewPath* path, BkDir* tree, BkFileBase** base);
void freeDirToWriteContents(DirToWrite* dir);
void freePathContents(NewPath* path);
int getLastNameFromPath(const char* srcPathAndName, char* lastName);
int makeNewPathFromString(const char* strPath, NewPath* pathPath);
bool nameIsValid(const char* name);
bool nameIsValid9660(const char* name);
void printDirToWrite(DirToWrite* dir, int level, int filenameTypes);

#endif
