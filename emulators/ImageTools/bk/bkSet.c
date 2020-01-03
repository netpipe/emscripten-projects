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

#include <strings.h>
#include <string.h>
#include <stdio.h>

#include "bk.h"
#include "bkSet.h"
#include "bkDelete.h"
#include "bkPath.h"
#include "bkError.h"
#include "bkIoWrappers.h"

void bk_cancel_operation(VolInfo* volInfo)
{
    volInfo->stopOperation = true;
}

/*******************************************************************************
* bk_destroy_vol_info()
* Frees any memory refered to by volinfo.
* If an image was open for reading closes it.
* Does not reinitialize the structure.
* */
void bk_destroy_vol_info(VolInfo* volInfo)
{
    BkHardLink* currentLink;
    BkHardLink* nextLink;

    deleteDirContents(volInfo, &(volInfo->dirTree));
    
    if(volInfo->bootRecordPathAndName != NULL)
        free(volInfo->bootRecordPathAndName);
    
    if(volInfo->imageForReading > 0)
        bkClose(volInfo->imageForReading);
    
    currentLink = volInfo->fileLocations;
    while(currentLink != NULL)
    {
        nextLink = currentLink->next;
        free(currentLink);
        currentLink = nextLink;
    }
}

/*******************************************************************************
* bk_init_vol_info()
*
* */
int bk_init_vol_info(VolInfo* volInfo, bool scanForDuplicateFiles)
{
    memset(volInfo, 0, sizeof(VolInfo));
    
    volInfo->dirTree.base.posixFileMode = 040755;
    volInfo->posixFileDefaults = 0100644;
    volInfo->posixDirDefaults = 040755;
    
    volInfo->scanForDuplicateFiles = scanForDuplicateFiles;
    
    return 1;
}

/*******************************************************************************
* bk_rename()
* Rename the file/dir.
* */
int bk_rename(VolInfo* volInfo, const char* srcPathAndName, 
              const char* newName)
{
    int rc;
    NewPath srcPath;
    BkDir* parentDir;
    bool dirFound;
    BkFileBase* child;
    bool done;
    size_t newNameLen;
    
    newNameLen = strlen(newName);
    
    if(newNameLen > NCHARS_FILE_ID_MAX_STORE - 1)
        return BKERROR_MAX_NAME_LENGTH_EXCEEDED;
    if(newNameLen == 0)
        return BKERROR_BLANK_NAME;
    if( !nameIsValid(newName) )
        return BKERROR_NAME_INVALID_CHAR;
    
    rc = makeNewPathFromString(srcPathAndName, &srcPath);
    if(rc <= 0)
    {
        freePathContents(&srcPath);
        return rc;
    }
    
    if(srcPath.numChildren == 0)
    {
        freePathContents(&srcPath);
        return BKERROR_RENAME_ROOT;
    }
    
    if( strcmp(srcPath.children[srcPath.numChildren - 1], newName) == 0 )
    /* rename to the same name, ignore silently */
        return 1;
    
    /* i want the parent directory */
    srcPath.numChildren--;
    dirFound = findDirByNewPath(&srcPath, &(volInfo->dirTree), &parentDir);
    srcPath.numChildren++;
    if(!dirFound)
    {
        freePathContents(&srcPath);
        return BKERROR_DIR_NOT_FOUND_ON_IMAGE;
    }
    
    done = false;
    
    child = parentDir->children;
    while(child != NULL && !done)
    {
        if(itemIsInDir(newName, parentDir))
            return BKERROR_DUPLICATE_RENAME;
        
        if(strcmp(child->name, srcPath.children[srcPath.numChildren - 1]) == 0)
        {
            strcpy(child->name, newName);
            
            done = true;
        }
        
        child = child->next;
    }
    
    freePathContents(&srcPath);
    
    if(done)
        return 1;
    else
        return BKERROR_ITEM_NOT_FOUND_ON_IMAGE;
}

/*******************************************************************************
* bk_set_boot_file()
* Set a file on the image to be the no-emulation boot record for el torito.
* */
int bk_set_boot_file(VolInfo* volInfo, const char* srcPathAndName)
{
    int rc;
    NewPath path;
    BkDir* srcDirInTree;
    BkFileBase* child;
    bool found;
    
    rc = makeNewPathFromString(srcPathAndName, &path);
    if(rc <= 0)
    {
        freePathContents(&path);
        return rc;
    }
    
    path.numChildren--;
    found = findDirByNewPath(&path, &(volInfo->dirTree), &srcDirInTree);
    if(!found)
        return BKERROR_DIR_NOT_FOUND_ON_IMAGE;
    path.numChildren++;
    
    /* FIND the file */
    found = false;
    child = srcDirInTree->children;
    while(child != NULL && !found)
    {
        if(strcmp(child->name, path.children[path.numChildren - 1]) == 0)
        {
            if( !IS_REG_FILE(child->posixFileMode) )
            {
                freePathContents(&path);
                return BKERROR_NOT_REG_FILE_FOR_BR;
            }
            
            found = true;
            
            volInfo->bootMediaType = BOOT_MEDIA_NO_EMULATION;
            
            volInfo->bootRecordSize = BK_FILE_PTR(child)->size;
            
            if(volInfo->bootRecordPathAndName != NULL)
            {
                free(volInfo->bootRecordPathAndName);
                volInfo->bootRecordPathAndName = NULL;
            }
            
            volInfo->bootRecordIsVisible = true;
            
            volInfo->bootRecordOnImage = BK_FILE_PTR(child);
        }
        
        child = child->next;
    }
    if(!found)
    {
        freePathContents(&path);
        return BKERROR_FILE_NOT_FOUND_ON_IMAGE;
    }
    /* END FIND the file */
    
    freePathContents(&path);
    
    return 1;
}

void bk_set_follow_symlinks(VolInfo* volInfo, bool doFollow)
{
    volInfo->followSymLinks = doFollow;
}

/*******************************************************************************
* bk_get_sermissions()
* public function
* sets the permissions (not all of the posix info) for an item (file, dir, etc.)
* */
int bk_set_permissions(VolInfo* volInfo, const char* pathAndName, 
                       mode_t permissions)
{
    int rc;
    NewPath srcPath;
    BkFileBase* base;
    bool itemFound;
    
    rc = makeNewPathFromString(pathAndName, &srcPath);
    if(rc <= 0)
    {
        freePathContents(&srcPath);
        return rc;
    }
    
    itemFound = findBaseByNewPath(&srcPath, &(volInfo->dirTree), &base);
    
    freePathContents(&srcPath);
    
    if(!itemFound)
        return BKERROR_ITEM_NOT_FOUND_ON_IMAGE;
    
    /* set all permission bits in posixFileMode to 0 */
    base->posixFileMode &= ~0777;
    
    /* copy permissions into posixFileMode */
    base->posixFileMode |= permissions & 0777;
    
    return 1;
}

/*******************************************************************************
* bk_set_publisher()
* Copies publisher into volInfo, a maximum of 128 characters.
* */
int bk_set_publisher(VolInfo* volInfo, const char* publisher)
{
    /* unfortunately some disks (e.g. Fedora 7) don't follow this rule
    if( !nameIsValid9660(publisher) )
        return BKERROR_NAME_INVALID_CHAR;*/
    
    strncpy(volInfo->publisher, publisher, 128);
    
    return 1;
}

/*******************************************************************************
* bk_set_vol_name()
* Copies volName into volInfo, a maximum of 32 characters.
* */
int bk_set_vol_name(VolInfo* volInfo, const char* volName)
{
    /* unfortunately some disks (e.g. Fedora 7) don't follow this rule
    if( !nameIsValid9660(volName) )
        return BKERROR_NAME_INVALID_CHAR;*/
    
    strncpy(volInfo->volId, volName, 32);
    
    return 1;
}

/*******************************************************************************
* itemIsInDir()
* checks the contents of a directory (files and dirs) to see whether it
* has an item named 
* */
bool itemIsInDir(const char* name, const BkDir* dir)
{
    BkFileBase* child;
    
    child = dir->children;
    while(child != NULL)
    {
        if(strcmp(child->name, name) == 0)
            return true;
        child = child->next;
    }
    
    return false;
}
