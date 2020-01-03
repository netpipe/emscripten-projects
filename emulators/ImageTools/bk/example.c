/******************************************************************************
* example.c
* Example for using bkisofs
* Author: Andrew Smith
* Compile with: cc example.c bk.a -o example
* */

#include <stdio.h>
#include <time.h>

/* need to include bk.h for access to bkisofs functions and structures */
#include "bk.h"

void addProgressUpdaterCbk(VolInfo* volInfo);
void fatalError(const char* message);
void printNameAndContents(BkFileBase* item, int numSpaces);
void readProgressUpdaterCbk(VolInfo* volInfo);
void writeProgressUpdaterCbk(VolInfo* volInfo, double percentComplete);

int main( int argv, char** argc)
{
    /* A variable of type VolInfo stores information about an image */
    VolInfo volInfo;
    /* bk functions return ints that need to be checked to see whether
    * the functions were successful or not */
    int rc;
    
    if(argv != 2)
        fatalError("Usage: example myfile.iso");
    
    /* initialise volInfo, set it up to scan for duplicate files */
    rc = bk_init_vol_info(&volInfo, true);
    if(rc <= 0)
        fatalError(bk_get_error_string(rc));
    
    /* open the iso file (supplied as argument 1) */
    rc = bk_open_image(&volInfo, argc[1]);
    if(rc <= 0)
        fatalError(bk_get_error_string(rc));
    
    /* read information about the volume (required before reading directory tree) */
    rc = bk_read_vol_info(&volInfo);
    if(rc <= 0)
        fatalError(bk_get_error_string(rc));
    
    /* read the directory tree */
    if(volInfo.filenameTypes & FNTYPE_ROCKRIDGE)
        rc = bk_read_dir_tree(&volInfo, FNTYPE_ROCKRIDGE, true, readProgressUpdaterCbk);
    else if(volInfo.filenameTypes & FNTYPE_JOLIET)
        rc = bk_read_dir_tree(&volInfo, FNTYPE_JOLIET, false, readProgressUpdaterCbk);
    else
        rc = bk_read_dir_tree(&volInfo, FNTYPE_9660, false, readProgressUpdaterCbk);
    if(rc <= 0)
        fatalError(bk_get_error_string(rc));
    
    /* add the file /etc/fstab to the root of the image */
    rc = bk_add(&volInfo, "/etc/fstab", "/", addProgressUpdaterCbk);
    if(rc <= 0)
        fatalError(bk_get_error_string(rc));
    
    /* print the entire directory tree */
    printNameAndContents(BK_BASE_PTR( &(volInfo.dirTree) ), 0);
    
    /* save the new ISO as /tmp/example.iso */
    /* note that bkisofs will print some stuff to stdout when writing an ISO */
    rc = bk_write_image("/tmp/example.iso", &volInfo, time(NULL),
                        FNTYPE_9660 | FNTYPE_ROCKRIDGE | FNTYPE_JOLIET,
                        writeProgressUpdaterCbk);
    
    /* we're finished with this ISO, so clean up */
    bk_destroy_vol_info(&volInfo);
    
    return 0;
}

/* you can use this to update a progress bar or something */
void addProgressUpdaterCbk(VolInfo* volInfo)
{
    printf("Add progress updater\n");
}

void fatalError(const char* message)
{
    printf("Fatal error: %s\n", message);
    exit(1);
}

void printNameAndContents(BkFileBase* base, int numSpaces)
{
    int count;
    
    /* print the spaces (indentation, for prettyness) */
    for(count = 0; count < numSpaces; count++)
        printf(" ");
    
    if(IS_DIR(base->posixFileMode))
    {
        /* print name of the directory */
        printf("%s (directory)\n", base->name);
        
        /* print all the directory's children */
        BkFileBase* child = BK_DIR_PTR(base)->children;
        while(child != NULL)
        {
            printNameAndContents(child, numSpaces + 2);
            child = child->next;
        }
    }
    else if(IS_REG_FILE(base->posixFileMode))
    {
        /* print name and size of the file */
        printf("%s (regular file), size %u\n", base->name, BK_FILE_PTR(base)->size);
    }
    else if(IS_SYMLINK(base->posixFileMode))
    {
        /* print name and target of the symbolic link */
        printf("%s -> %s (symbolic link)\n", base->name, BK_SYMLINK_PTR(base)->target);
    }
}

/* you can use this to update a progress bar or something */
void readProgressUpdaterCbk(VolInfo* volInfo)
{
    printf("Read progress updater\n");
}

/* you can use this to update a progress bar or something */
void writeProgressUpdaterCbk(VolInfo* volInfo, double percentComplete)
{
    printf("Write progress updater: ~%.2lf%% complete\n", percentComplete);
}
