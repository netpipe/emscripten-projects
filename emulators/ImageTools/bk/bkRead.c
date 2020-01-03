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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

#include "bk.h"
#include "bkInternal.h"
#include "bkRead.h"
#include "bkRead7x.h"
#include "bkTime.h"
#include "bkError.h"
#include "bkLink.h"
#include "bkMisc.h"
#include "bkIoWrappers.h"

/* numbers as recorded on image */
#define VDTYPE_BOOT 0
#define VDTYPE_PRIMARY 1
#define VDTYPE_SUPPLEMENTARY 2
#define VDTYPE_VOLUMEPARTITION 3
#define VDTYPE_TERMINATOR 255

/* for el torito boot images */
#define NBYTES_VIRTUAL_SECTOR 512

/* this function is really just for use in readRockridgeSymlink()
* returns number of chars appended
* destMaxLen doesn't include '\0'
* if maxSrcLen is -1 tries to copy all of it */
size_t appendStringIfHaveRoom(char* dest, const char* src, size_t destMaxLen, 
                              size_t destCharsAlreadyUsed, int maxSrcLen)
{
    size_t srcLen;
    
    if(maxSrcLen == -1)
        srcLen = strlen(src);
    else
        srcLen = maxSrcLen;
    
    if(destCharsAlreadyUsed + srcLen > destMaxLen)
        return 0;
    
    strncat(dest, src, srcLen);
    
    return srcLen;
}

/*******************************************************************************
* bk_open_image()
* 
* */
int bk_open_image(VolInfo* volInfo, const char* filename)
{
    size_t len;
    
    volInfo->imageForReading = open(filename, O_RDONLY, 0);
    if(volInfo->imageForReading == -1)
    {
        volInfo->imageForReading = 0;
        return BKERROR_OPEN_READ_FAILED;
    }
    
    int rc;
    BkStatStruct statStruct;
    
    /* record inode number */
    rc = bkStat(filename, &statStruct);
    if(rc == -1)
        return BKERROR_STAT_FAILED;
    
    volInfo->imageForReadingInode = statStruct.st_ino;
    
    /* skip the first 150 sectors if the image is an NRG */
    len = strlen(filename);
    if( (filename[len - 3] == 'N' || filename[len - 3] == 'n') &&
        (filename[len - 2] == 'R' || filename[len - 2] == 'r') &&
        (filename[len - 1] == 'G' || filename[len - 1] == 'g') )
    {
        readSeekSet(volInfo, NBYTES_LOGICAL_BLOCK * 16, SEEK_SET);
    }
    
    return 1;
}

/*******************************************************************************
* bk_read_dir_tree()
* filenameType can be only one (do not | more then one)
* */
int bk_read_dir_tree(VolInfo* volInfo, int filenameType, 
                     bool keepPosixPermissions, 
                     void(*progressFunction)(VolInfo*))
{
    volInfo->progressFunction = progressFunction;
    
    if(filenameType == FNTYPE_ROCKRIDGE || filenameType == FNTYPE_9660)
        readSeekSet(volInfo, volInfo->pRootDrOffset, SEEK_SET);
    else /* if(filenameType == FNTYPE_JOLIET) */
        readSeekSet(volInfo, volInfo->sRootDrOffset, SEEK_SET);
    
    return readDir(volInfo, &(volInfo->dirTree), 
                   filenameType, keepPosixPermissions);
}

/*******************************************************************************
* bk_read_vol_info()
* public function to read volume information
* assumes pvd is first descriptor in set
* */
int bk_read_vol_info(VolInfo* volInfo)
{
    int rc;
    unsigned char vdType; /* to check what descriptor follows */
    bool haveMorePvd; /* to skip extra pvds */
    unsigned char escapeSequence[3]; /* only interested in a joliet sequence */
    char timeString[17]; /* for creation time */
    bk_off_t locationOfNextDescriptor;
    unsigned bootCatalogLocation; /* logical sector number */
    char elToritoSig[24];
    unsigned char bootMediaType;
    unsigned short bootRecordSize;
    unsigned bootRecordSectorNumber;
    
    /* vars for checking rockridge */
    unsigned realRootLoc; /* location of the root dr inside root dir */
    unsigned char recordLen; /* length of rood dr */
    unsigned char sPsUentry[7]; /* su entry SP */
    
    /* will always have this unless image is broken */
    volInfo->filenameTypes = FNTYPE_9660;
    
    /* might not have supplementary descriptor */
    volInfo->sRootDrOffset = 0;
    
    /* skip system area */
    readSeekSet(volInfo, NLS_SYSTEM_AREA * NBYTES_LOGICAL_BLOCK, SEEK_SET);
    
    /* READ PVD */
    /* make sure pvd exists */
    rc = read711(volInfo, &vdType);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    /* first descriptor must be primary */
    if(vdType != VDTYPE_PRIMARY)
        return BKERROR_VD_NOT_PRIMARY;
    
    readSeekSet(volInfo, 39, SEEK_CUR);
    
    rc = readRead(volInfo, volInfo->volId, 32);
    if(rc != 32)
        return BKERROR_READ_GENERIC;
    volInfo->volId[32] = '\0';
    stripSpacesFromEndOfString(volInfo->volId);
    
    readSeekSet(volInfo, 84, SEEK_CUR);
    
    /* am now at root dr */
    volInfo->pRootDrOffset = readSeekTell(volInfo);
    
    /* SEE if rockridge exists */
    readSeekSet(volInfo, 2, SEEK_CUR);
    
    rc = read733(volInfo, &realRootLoc);
    if(rc != 8)
        return BKERROR_READ_GENERIC;
    realRootLoc *= NBYTES_LOGICAL_BLOCK;
    
    readSeekSet(volInfo, realRootLoc, SEEK_SET);
    
    rc = read711(volInfo, &recordLen);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    if(recordLen >= 41)
    /* a minimum root with SP su field */
    {
        /* root dr has filename length of 1 */
        readSeekSet(volInfo, 33, SEEK_CUR);
        
        /* a rockridge root dr has an SP su entry here */
        
        rc = readRead(volInfo, &sPsUentry, 7);
        if(rc != 7)
            return BKERROR_READ_GENERIC;
        
        if( sPsUentry[0] == 0x53 && sPsUentry[1] == 0x50 &&
            sPsUentry[2] == 7 && 
            sPsUentry[4] == 0xBE && sPsUentry[5] == 0xEF )
        /* rockridge it is */
        {
            volInfo->filenameTypes |= FNTYPE_ROCKRIDGE;
        }
    }
    
    /* go back to where it was before trying rockridge */
    readSeekSet(volInfo, volInfo->pRootDrOffset, SEEK_SET);
    /* END SEE if rockridge exists */
    
    readSeekSet(volInfo, 162, SEEK_CUR);
    
    rc = readRead(volInfo, volInfo->publisher, 128);
    if(rc != 128)
        return BKERROR_READ_GENERIC;
    volInfo->publisher[128] = '\0';
    stripSpacesFromEndOfString(volInfo->publisher);
    
    rc = readRead(volInfo, volInfo->dataPreparer, 128);
    if(rc != 128)
        return BKERROR_READ_GENERIC;
    volInfo->dataPreparer[128] = '\0';
    stripSpacesFromEndOfString(volInfo->dataPreparer);
    
    readSeekSet(volInfo, 239, SEEK_CUR);
    
    rc = readRead(volInfo, timeString, 17);
    if(rc != 17)
        return BKERROR_READ_GENERIC;
    
    longStringToEpoch(timeString, &(volInfo->creationTime));
    
    /* skip the rest of the extent */
    readSeekSet(volInfo, 1218, SEEK_CUR);
    /* END READ PVD */
    
    /* SKIP all extra copies of pvd */
    haveMorePvd = true;
    while(haveMorePvd)
    {
        rc = read711(volInfo, &vdType);
        if(rc != 1)
            return BKERROR_READ_GENERIC;
        
        if(vdType == VDTYPE_PRIMARY)
        {
            readSeekSet(volInfo, 2047, SEEK_CUR);
        }
        else
        {
            readSeekSet(volInfo, -1, SEEK_CUR);
            haveMorePvd = false;
        }
    }
    /* END SKIP all extra copies of pvd */
    
    /* TRY read boot record */
   
    locationOfNextDescriptor = readSeekTell(volInfo) + 2048;
    
    rc = read711(volInfo, &vdType);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    if(vdType == VDTYPE_BOOT)
    {
        
        readSeekSet(volInfo, 6, SEEK_CUR);
        
        rc = readRead(volInfo, elToritoSig, 24);
        if(rc != 24)
            return BKERROR_READ_GENERIC;
        elToritoSig[23] = '\0'; /* just in case */
        
        if(strcmp(elToritoSig, "EL TORITO SPECIFICATION") == 0)
        /* el torito confirmed */
        {
            readSeekSet(volInfo, 40, SEEK_CUR);
            
            rc = read731(volInfo, &bootCatalogLocation);
            if(rc != 4)
                return BKERROR_READ_GENERIC;
            
            readSeekSet(volInfo, bootCatalogLocation * NBYTES_LOGICAL_BLOCK, SEEK_SET);
            
            /* skip validation entry */
            readSeekSet(volInfo, 32, SEEK_CUR);
            
            /* skip boot indicator */
            readSeekSet(volInfo, 1, SEEK_CUR);
            
            rc = readRead(volInfo, &bootMediaType, 1);
            if(rc != 1)
                return BKERROR_READ_GENERIC;
            if(bootMediaType == 0)
                volInfo->bootMediaType = BOOT_MEDIA_NO_EMULATION;
            else if(bootMediaType == 1)
                volInfo->bootMediaType = BOOT_MEDIA_1_2_FLOPPY;
            else if(bootMediaType == 2)
                volInfo->bootMediaType = BOOT_MEDIA_1_44_FLOPPY;
            else if(bootMediaType == 3)
                volInfo->bootMediaType = BOOT_MEDIA_2_88_FLOPPY;
            else if(bootMediaType == 4)
            {
                /* !! print warning */
                printf("hard disk boot emulation not supported\n");
                volInfo->bootMediaType = BOOT_MEDIA_NONE;
            }
            else
            {
                /* !! print warning */
                printf("unknown boot media type on iso\n");
                volInfo->bootMediaType = BOOT_MEDIA_NONE;
            }
            
            /* skip load segment, system type and unused byte */
            readSeekSet(volInfo, 4, SEEK_CUR);
            
            rc = read721(volInfo, &bootRecordSize);
            if(rc != 2)
                return BKERROR_READ_GENERIC;
            volInfo->bootRecordSize = bootRecordSize;
            
            if(volInfo->bootMediaType == BOOT_MEDIA_NO_EMULATION)
                volInfo->bootRecordSize *= NBYTES_VIRTUAL_SECTOR;
            else if(volInfo->bootMediaType == BOOT_MEDIA_1_2_FLOPPY)
                volInfo->bootRecordSize = 1200 * 1024;
            else if(volInfo->bootMediaType == BOOT_MEDIA_1_44_FLOPPY)
                volInfo->bootRecordSize = 1440 * 1024;
            else if(volInfo->bootMediaType == BOOT_MEDIA_2_88_FLOPPY)
                volInfo->bootRecordSize = 2880 * 1024;
            
            volInfo->bootRecordIsOnImage = true;
            
            rc = read731(volInfo, &bootRecordSectorNumber);
            if(rc != 4)
                return BKERROR_READ_GENERIC;
            volInfo->bootRecordOffset = bootRecordSectorNumber * 
                                        NBYTES_LOGICAL_BLOCK;
        }
        else
            /* !! print warning */
            printf("err, boot record not el torito\n");
        
        /* go to the sector after the boot record */
        readSeekSet(volInfo, locationOfNextDescriptor, SEEK_SET);
    }
    else
    /* not boot record */
    {
        /* go back */
        readSeekSet(volInfo, -1, SEEK_CUR);
    }
    /* END TRY read boot record */
    
    /* TRY read svd */
    rc = read711(volInfo, &vdType);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    if(vdType == VDTYPE_SUPPLEMENTARY)
    /* make sure it's joliet (by escape sequence) */
    {
        readSeekSet(volInfo, 87, SEEK_CUR);
        
        rc = readRead(volInfo, escapeSequence, 3);
        if(rc != 3)
            return BKERROR_READ_GENERIC;
        
        if( (escapeSequence[0] == 0x25 && escapeSequence[1] == 0x2F &&
             escapeSequence[2] == 0x40) ||
            (escapeSequence[0] == 0x25 && escapeSequence[1] == 0x2F &&
             escapeSequence[2] == 0x43) ||
            (escapeSequence[0] == 0x25 && escapeSequence[1] == 0x2F &&
             escapeSequence[2] == 0x45) )
        /* is indeed joliet */
        {
            readSeekSet(volInfo, 65, SEEK_CUR);
            
            volInfo->sRootDrOffset = readSeekTell(volInfo);
            
            volInfo->filenameTypes |= FNTYPE_JOLIET;
        }
    }
    /* END TRY read svd */
    
    return 1;
}

/*******************************************************************************
* dirDrFollows()
* checks whether the next directory record is for a directory (not a file)
* returns 2 if it does, and 1 if it does not
* */
int dirDrFollows(VolInfo* volInfo)
{
    unsigned char fileFlags;
    bk_off_t origPos;
    int rc;
    
    origPos = readSeekTell(volInfo);
    
    readSeekSet(volInfo, 25, SEEK_CUR);
    
    rc = read711(volInfo, &fileFlags);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    readSeekSet(volInfo, origPos, SEEK_SET);
    
    if((fileFlags >> 1 & 1) == 1)
        return 2;
    else
        return 1;
}

/*******************************************************************************
* haveNextRecordInSector()
* If a directory record won't fit into what's left in a logical block, the rest
* of the block is filled with 0s. This function checks whether that's the case.
* If the next byte is zero returns false otherwise true
* File position remains unchanged
* Also returns false on read error */
bool haveNextRecordInSector(VolInfo* volInfo)
{
    bk_off_t origPos;
    char testByte;
    int rc;
    
    origPos = readSeekTell(volInfo);
    
    rc = readRead(volInfo, &testByte, 1);
    if(rc != 1)
        return false;
    
    readSeekSet(volInfo, origPos, SEEK_SET);
    
    return (testByte == 0) ? false : true;
}

/*******************************************************************************
* readDir()
* Reads a directory record for a directory (not a file)
* Do not use this to read self or parent records unless it's the following:
* - if the root dr (inside vd) is read, it's filename will be ""
* filenameType can be only one (do not | more then one)
*
* note to self: directory identifiers do not end with ";1"
*
* */
int readDir(VolInfo* volInfo, BkDir* dir, int filenameType, 
            bool keepPosixPermissions)
{
    int rc;
    unsigned char recordLength;
    unsigned locExtent; /* to know where to go before readDir9660() */
    unsigned lenExtent; /* parameter to readDirContents() */
    unsigned char lenFileId9660; /* also len joliet fileid (bytes) */
    int lenSU; /* calculated as recordLength - 33 - lenFileId9660 */
    bk_off_t origPos;
    
    /* should anything fail, will still be safe to delete dir, this also
    * needs to be done before calling readDirContents() (now is good) */
    dir->children = NULL;
    
    if(volInfo->stopOperation)
        return BKERROR_OPER_CANCELED_BY_USER;
    
    maybeUpdateProgress(volInfo);
    
    rc = readRead(volInfo, &recordLength, 1);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    readSeekSet(volInfo, 1, SEEK_CUR);
    
    rc = read733(volInfo, &locExtent);
    if(rc != 8)
        return BKERROR_READ_GENERIC;
    
    rc = read733(volInfo, &lenExtent);
    if(rc != 8)
        return BKERROR_READ_GENERIC;
    
    readSeekSet(volInfo, 14, SEEK_CUR);
    
    rc = readRead(volInfo, &lenFileId9660, 1);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    lenSU = recordLength - 33 - lenFileId9660;
    if(lenFileId9660 % 2 == 0)
        lenSU -= 1;
    
    /* READ directory name */
    if(volInfo->rootRead)
    {
        bk_off_t posBeforeName = readSeekTell(volInfo);
        
        rc = readRead(volInfo, BK_BASE_PTR(dir)->name, lenFileId9660);
        if(rc != lenFileId9660)
            return BKERROR_READ_GENERIC;
        BK_BASE_PTR(dir)->name[lenFileId9660] = '\0';
        
        /* record 9660 name for writing later */
        strncpy(BK_BASE_PTR(dir)->original9660name, BK_BASE_PTR(dir)->name, 14);
        BK_BASE_PTR(dir)->original9660name[14] = '\0';
        
        /* skip padding field if it's there */
        if(lenFileId9660 % 2 == 0)
            readSeekSet(volInfo, 1, SEEK_CUR);
        
        if(filenameType != FNTYPE_9660)
            readSeekSet(volInfo, posBeforeName, SEEK_SET);
    }
    
    if(filenameType == FNTYPE_JOLIET)
    {
        if(volInfo->rootRead)
        {
            char nameAsOnDisk[UCHAR_MAX];
            /* in the worst possible case i'll use 129 bytes for this: */
            char nameInAscii[UCHAR_MAX];
            int ucsCount, byteCount;
            
            /* ucs2 byte count must be even */
            if(lenFileId9660 % 2 != 0)
                return BKERROR_INVALID_UCS2;
            
            rc = readRead(volInfo, nameAsOnDisk, lenFileId9660);
            if(rc != lenFileId9660)
                return BKERROR_READ_GENERIC;
            
            for(ucsCount = 1, byteCount = 0; ucsCount < lenFileId9660;
                ucsCount += 2, byteCount += 1)
            {
                nameInAscii[byteCount] = nameAsOnDisk[ucsCount];
            }
            nameInAscii[byteCount] = '\0';
            
            strncpy(BK_BASE_PTR(dir)->name, nameInAscii, lenFileId9660);
            BK_BASE_PTR(dir)->name[lenFileId9660] = '\0';
            
            /* padding field */
            if(lenFileId9660 % 2 == 0)
                readSeekSet(volInfo, 1, SEEK_CUR);
        }
    }
    else if(filenameType == FNTYPE_ROCKRIDGE)
    {
        if(volInfo->rootRead)
        {
            /* skip 9660 filename */
            readSeekSet(volInfo, lenFileId9660, SEEK_CUR);
            /* skip padding field */
            if(lenFileId9660 % 2 == 0)
                readSeekSet(volInfo, 1, SEEK_CUR);
            
            rc = readRockridgeFilename(volInfo, BK_BASE_PTR(dir)->name, lenSU, 0);
            if(rc < 0)
                return rc;
        }
    }
    else if(filenameType != FNTYPE_9660)
        return BKERROR_UNKNOWN_FILENAME_TYPE;
    /* END READ directory name */
    
    if(keepPosixPermissions)
    {
        if( !(volInfo->rootRead) )
        {
            unsigned char realRootRecordLen;
            
            origPos = readSeekTell(volInfo);
            
            /* go to real root record */
            readSeekSet(volInfo, locExtent * NBYTES_LOGICAL_BLOCK, SEEK_SET);
            
            /* read record length */
            rc = readRead(volInfo, &realRootRecordLen, 1);
            if(rc != 1)
                return BKERROR_READ_GENERIC;
            
            /* go to sys use fields */
            readSeekSet(volInfo, 33, SEEK_CUR);
            
            rc = readPosixFileMode(volInfo, &(BK_BASE_PTR(dir)->posixFileMode), realRootRecordLen - 34);
            if(rc <= 0)
                return rc;
            
            /* return */
            readSeekSet(volInfo, origPos, SEEK_SET);
        }
        else
        {
            rc = readPosixFileMode(volInfo, &(BK_BASE_PTR(dir)->posixFileMode), lenSU);
            if(rc <= 0)
                return rc;
        }
    }
    else
    {
        /* this is good for root also */
        BK_BASE_PTR(dir)->posixFileMode = volInfo->posixDirDefaults;
    }
    
    readSeekSet(volInfo, lenSU, SEEK_CUR);
    
    origPos = readSeekTell(volInfo);
    
    readSeekSet(volInfo, locExtent * NBYTES_LOGICAL_BLOCK, SEEK_SET);
    
    volInfo->rootRead = true;
    
    rc = readDirContents(volInfo, dir, lenExtent, filenameType, keepPosixPermissions);
    if(rc < 0)
        return rc;
    
    readSeekSet(volInfo, origPos, SEEK_SET);
    
    return recordLength;
}

/*******************************************************************************
* readDirContents()
* Reads the extent pointed to from a directory record of a directory.
* size is number of bytes
* */
int readDirContents(VolInfo* volInfo, BkDir* dir, unsigned size, 
                    int filenameType, bool keepPosixPermissions)
{
    int rc;
    unsigned bytesRead = 0;
    unsigned childrenBytesRead;
    BkFileBase** nextChild; /* pointer to pointer to modify pointer :) */
    
    /* skip self and parent */
    rc = skipDR(volInfo);
    if(rc <= 0)
        return rc;
    bytesRead += rc;
    rc = skipDR(volInfo);
    if(rc <= 0)
        return rc;
    bytesRead += rc;
    
    nextChild = &(dir->children);
    childrenBytesRead = 0;
    while(childrenBytesRead + bytesRead < size)
    {
        if(haveNextRecordInSector(volInfo))
        /* read it */
        {
            int recordLength;
            
            rc = dirDrFollows(volInfo);
            if(rc == 2)
            /* directory descriptor record */
            {
                *nextChild = malloc(sizeof(BkDir));
                if(*nextChild == NULL)
                    return BKERROR_OUT_OF_MEMORY;
                
                memset(*nextChild, 0, sizeof(BkDir));
                
                recordLength = readDir(volInfo, BK_DIR_PTR(*nextChild), 
                                       filenameType, keepPosixPermissions);
                if(recordLength < 0)
                    return recordLength;
            }
            else
            /* file descriptor record */
            {
                BkFileBase* specialFile;
                
                /* assume it's a file for now */
                *nextChild = malloc(sizeof(BkFile));
                if(*nextChild == NULL)
                    return BKERROR_OUT_OF_MEMORY;
                
                memset(*nextChild, 0, sizeof(BkFile));
                
                recordLength = readFileInfo(volInfo, BK_FILE_PTR(*nextChild), 
                                            filenameType, keepPosixPermissions, 
                                            &specialFile);
                if(recordLength < 0)
                    return recordLength;
                
                if(specialFile != NULL)
                /* it's a special file, replace the allocated BkFile */
                {
                    free(*nextChild);
                    *nextChild = specialFile;
                }
            }
            
            childrenBytesRead += recordLength;
            
            nextChild = &((*nextChild)->next);
            *nextChild = NULL;
        }
        else
        /* read zeroes until get to next record (that would be in the next
        *  sector btw) or get to the end of data (dir->self.dataLength) */
        {
            char testByte;
            bk_off_t origPos;
            
            do
            {
                origPos = readSeekTell(volInfo);
                
                rc = readRead(volInfo, &testByte, 1);
                if(rc != 1)
                    return BKERROR_READ_GENERIC;
                
                if(testByte != 0)
                {
                    readSeekSet(volInfo, origPos, SEEK_SET);
                    break;
                }
                
                childrenBytesRead += 1;
                
            } while (childrenBytesRead + bytesRead < size);
        }
    }
    
    return bytesRead;
}

/*******************************************************************************
* readFileInfo()
* Reads the directory record for a file
* */
int readFileInfo(VolInfo* volInfo, BkFile* file, int filenameType, 
                 bool keepPosixPermissions, BkFileBase** specialFile)
{
    int rc;
    unsigned char recordLength;
    unsigned locExtent; /* block num where the file is */
    unsigned lenExtent; /* in bytes */
    unsigned char lenFileId9660; /* also len joliet fileid (bytes) */
    int lenSU; /* calculated as recordLength - 33 - lenFileId9660 */
    bk_off_t posBeforeName;
    char nameAsOnDisk[UCHAR_MAX + 1];
    
    /* so if anything failes it's still safe to delete file */
    file->pathAndName = NULL;
    
    if(volInfo->stopOperation)
        return BKERROR_OPER_CANCELED_BY_USER;
    
    maybeUpdateProgress(volInfo);
    
    *specialFile = NULL;
    
    rc = readRead(volInfo, &recordLength, 1);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    readSeekSet(volInfo, 1, SEEK_CUR);
    
    rc = read733(volInfo, &locExtent);
    if(rc != 8)
        return BKERROR_READ_GENERIC;
    
    rc = read733(volInfo, &lenExtent);
    if(rc != 8)
        return BKERROR_READ_GENERIC;
    
    /* The length of isolinux.bin given in the initial/default entry of
    * the el torito boot catalog does not match the actual length of the file
    * but apparently when executed by the bios that's not a problem.
    * However, if i ever want to read that file myself, i need 
    * the length proper.
    * So i'm looking for a file that starts in the same logical sector as the
    * boot record from the initial/default entry. */
    if(volInfo->bootMediaType == BOOT_MEDIA_NO_EMULATION && 
       locExtent == volInfo->bootRecordOffset / NBYTES_LOGICAL_BLOCK)
    {
        volInfo->bootRecordSize = lenExtent;
        
        volInfo->bootRecordIsVisible = true;
        volInfo->bootRecordOnImage = file;
    }
    
    readSeekSet(volInfo, 14, SEEK_CUR);
    
    rc = readRead(volInfo, &lenFileId9660, 1);
    if(rc != 1)
        return BKERROR_READ_GENERIC;
    
    lenSU = recordLength - 33 - lenFileId9660;
    if(lenFileId9660 % 2 == 0)
        lenSU -= 1;
    
    /* READ 9660 name */
    posBeforeName = readSeekTell(volInfo);
    
    rc = readRead(volInfo, nameAsOnDisk, lenFileId9660);
    if(rc != lenFileId9660)
        return BKERROR_READ_GENERIC;
    nameAsOnDisk[lenFileId9660] = '\0';
    
    /* record 9660 name for writing later */
    strncpy(BK_BASE_PTR(file)->original9660name, nameAsOnDisk, 14);
    BK_BASE_PTR(file)->original9660name[14] = '\0';
    
    removeCrapFromFilename(nameAsOnDisk, lenFileId9660);
    
    strncpy(BK_BASE_PTR(file)->name, nameAsOnDisk, NCHARS_FILE_ID_MAX_STORE - 1);
    BK_BASE_PTR(file)->name[NCHARS_FILE_ID_MAX_STORE - 1] = '\0';
    
    /* padding field */
    if(lenFileId9660 % 2 == 0)
        readSeekSet(volInfo, 1, SEEK_CUR);
    
    if(filenameType != FNTYPE_9660)
            readSeekSet(volInfo, posBeforeName, SEEK_SET);
    /* END READ 9660 name */
    
    if(filenameType == FNTYPE_JOLIET)
    {
        char nameAsOnDisk[UCHAR_MAX];
        /* in the worst possible case i'll use 129 bytes for this: */
        char nameInAscii[UCHAR_MAX];
        int ucsCount, byteCount;
        
        if(lenFileId9660 % 2 != 0)
            return BKERROR_INVALID_UCS2;
        
        rc = readRead(volInfo, nameAsOnDisk, lenFileId9660);
        if(rc != lenFileId9660)
            return BKERROR_READ_GENERIC;
        
        for(ucsCount = 1, byteCount = 0; ucsCount < lenFileId9660;
            ucsCount += 2, byteCount += 1)
        {
            nameInAscii[byteCount] = nameAsOnDisk[ucsCount];
        }
        
        removeCrapFromFilename(nameInAscii, lenFileId9660 / 2);
        
        if( strlen(nameInAscii) > NCHARS_FILE_ID_MAX_STORE - 1 )
            return BKERROR_MAX_NAME_LENGTH_EXCEEDED;
        
        strncpy(BK_BASE_PTR(file)->name, nameInAscii, NCHARS_FILE_ID_MAX_STORE - 1);
        BK_BASE_PTR(file)->name[NCHARS_FILE_ID_MAX_STORE - 1] = '\0';
        
        /* padding field */
        if(lenFileId9660 % 2 == 0)
            readSeekSet(volInfo, 1, SEEK_CUR);
    }
    else if(filenameType == FNTYPE_ROCKRIDGE)
    {
        /* skip 9660 filename */
        readSeekSet(volInfo, lenFileId9660, SEEK_CUR);
        /* skip padding field */
        if(lenFileId9660 % 2 == 0)
            readSeekSet(volInfo, 1, SEEK_CUR);
        
        rc = readRockridgeFilename(volInfo, BK_BASE_PTR(file)->name, lenSU, 0);
        if(rc < 0)
            return rc;
    }
    else if(filenameType != FNTYPE_9660)
        return BKERROR_UNKNOWN_FILENAME_TYPE;
    
    if(keepPosixPermissions)
    {
        rc = readPosixFileMode(volInfo, &(BK_BASE_PTR(file)->posixFileMode), lenSU);
        if(rc < 0)
            return rc;
    }
    else
    {
        BK_BASE_PTR(file)->posixFileMode = volInfo->posixFileDefaults;
    }
    //~ printf("'%s' %X\n", BK_BASE_PTR(file)->name, BK_BASE_PTR(file)->posixFileMode);
    rc = readRockridgeSymlink(volInfo, (BkSymLink**)specialFile, lenSU);
    if(rc < 0)
        return rc;
    
    if(*specialFile != NULL)
    /* the file is actually a symbolic link */
    {
        strcpy((*specialFile)->name, BK_BASE_PTR(file)->name);
        strcpy((*specialFile)->original9660name, BK_BASE_PTR(file)->original9660name);
        /* apparently permissions for symbolic links are never used */
        (*specialFile)->posixFileMode = 0120777;
    }
    
    if(volInfo->scanForDuplicateFiles)
    {
        BkHardLink* newLink;
        
        rc = findInHardLinkTable(volInfo, locExtent * NBYTES_LOGICAL_BLOCK, NULL,
                                 lenExtent, true, &newLink);
        if(rc < 0)
            return rc;
        
        if(newLink == NULL)
        /* not found */
        {
            rc = addToHardLinkTable(volInfo, locExtent * NBYTES_LOGICAL_BLOCK, 
                                    NULL, lenExtent, true, &newLink);
            if(rc < 0)
                return rc;
        }
        
        file->location = newLink;
    }
    
    readSeekSet(volInfo, lenSU, SEEK_CUR);
    
    file->onImage = true;
    file->position = ((bk_off_t)locExtent) * ((bk_off_t)NBYTES_LOGICAL_BLOCK);
    file->size = lenExtent;
    
    return recordLength;
}

/*******************************************************************************
* readPosixFileMode()
* looks for the PX system use field and gets the permissions field out of it
* */
int readPosixFileMode(VolInfo* volInfo, unsigned* posixFileMode, int lenSU)
{
    bk_off_t origPos;
    unsigned char* suFields;
    int rc;
    bool foundPosix;
    bool foundCE;
    int count;
    unsigned logicalBlockOfCE;
    unsigned offsetInLogicalBlockOfCE;
    unsigned lengthOfCE; /* in bytes */
    
    suFields = malloc(lenSU);
    if(suFields == NULL)
        return BKERROR_OUT_OF_MEMORY;
    
    origPos = readSeekTell(volInfo);
    
    rc = readRead(volInfo, suFields, lenSU);
    
    if(rc != lenSU)
        return BKERROR_READ_GENERIC;
    count = 0;
    foundPosix = false;
    foundCE = false;
    while(count < lenSU && !foundPosix)
    {
        if(suFields[count] == 0)
        /* not an SU field, mkisofs sometimes has a trailing 0 in the directory
        * record and this is the easiest way to ignore it */
            break;
        
        if(suFields[count] == 'P' && suFields[count + 1] == 'X')
        {
            //~ printf("%X %X %X %X\n", *(suFields + count + 4), *(suFields + count + 5), *(suFields + count + 6), *(suFields + count + 7));
            read733FromCharArray(suFields + count + 4, posixFileMode);
            
            /* not interested in anything else from this field */
            
            foundPosix = true;
        }
        else if(suFields[count] == 'C' && suFields[count + 1] == 'E')
        {
            foundCE = true;
            read733FromCharArray(suFields + count + 4, &logicalBlockOfCE);
            read733FromCharArray(suFields + count + 12, &offsetInLogicalBlockOfCE);
            read733FromCharArray(suFields + count + 20, &lengthOfCE);
        }
        
        /* skip su record */
        count += suFields[count + 2];
    }
    
    free(suFields);
    readSeekSet(volInfo, origPos, SEEK_SET);
    
    if(!foundPosix)
    {
        if(!foundCE)
            return BKERROR_NO_POSIX_PRESENT;
        else
        {
            readSeekSet(volInfo, logicalBlockOfCE * NBYTES_LOGICAL_BLOCK + 
                        offsetInLogicalBlockOfCE, SEEK_SET);
            rc = readPosixFileMode(volInfo, posixFileMode, lengthOfCE);
            
            readSeekSet(volInfo, origPos, SEEK_SET);
            
            return rc;
        }
    }
    
    return 1;
}

/*******************************************************************************
* readRockridgeFilename()
* Finds the NM entry in the system use fields and reads a maximum of
* NCHARS_FILE_ID_MAX_STORE characters from it (truncates if necessary).
* The continue system use field is not implemented so if the name is not in
* this directory record, the function returns a failure.
* Leaves the file pointer where it was.
*/
int readRockridgeFilename(VolInfo* volInfo, char* dest, int lenSU, 
                          unsigned numCharsReadAlready)
{
    bk_off_t origPos;
    unsigned char* suFields;
    int rc;
    int count;
    int lengthThisNM;
    int usableLenThisNM;
    bool foundName;
    bool nameContinues; /* in another NM entry */
    bool foundCE;
    unsigned logicalBlockOfCE;
    unsigned offsetInLogicalBlockOfCE;
    unsigned lengthOfCE; /* in bytes */
    
    suFields = malloc(lenSU);
    if(suFields == NULL)
        return BKERROR_OUT_OF_MEMORY;
    
    origPos = readSeekTell(volInfo);
    
    rc = readRead(volInfo, suFields, lenSU);
    if(rc != lenSU)
    {
        free(suFields);
        return BKERROR_READ_GENERIC;
    }
    
    count = 0;
    foundName = false;
    nameContinues = false;
    foundCE = false;
    while(count < lenSU)
    {
        if(suFields[count] == 0)
        /* not an SU field, mkisofs sometimes has a trailing 0 in the directory
        * record and this is the easiest way to ignore it */
            break;
        
        if(suFields[count] == 'N' && suFields[count + 1] == 'M')
        {
            lengthThisNM = suFields[count + 2] - 5;
            
            /* the data structures cannot handle filenames longer than 
            * NCHARS_FILE_ID_MAX_STORE so in case the image contains an 
            * invalid, long filename, truncate it rather than corrupt memory */
            if(lengthThisNM + numCharsReadAlready > NCHARS_FILE_ID_MAX_STORE - 1)
                usableLenThisNM = NCHARS_FILE_ID_MAX_STORE - numCharsReadAlready - 1;
            else
                usableLenThisNM = lengthThisNM;
            
            strncpy(dest + numCharsReadAlready, (char*)suFields + count + 5, usableLenThisNM);
            dest[usableLenThisNM + numCharsReadAlready] = '\0';
            
            numCharsReadAlready += usableLenThisNM;
            
            foundName = true;
            nameContinues = suFields[count + 4] & 0x01; /* NM 'continue' flag */
        }
        else if(suFields[count] == 'C' && suFields[count + 1] == 'E')
        {
            foundCE = true;
            read733FromCharArray(suFields + count + 4, &logicalBlockOfCE);
            read733FromCharArray(suFields + count + 12, &offsetInLogicalBlockOfCE);
            read733FromCharArray(suFields + count + 20, &lengthOfCE);
        }
        
        /* skip su record */
        count += suFields[count + 2];
    }
    
    free(suFields);
    readSeekSet(volInfo, origPos, SEEK_SET);
    
    if( !foundName || (foundName && nameContinues) )
    {
        if(!foundCE)
            return BKERROR_RR_FILENAME_MISSING;
        else
        {
            readSeekSet(volInfo, 
                        logicalBlockOfCE * NBYTES_LOGICAL_BLOCK + offsetInLogicalBlockOfCE, 
                        SEEK_SET);
            rc = readRockridgeFilename(volInfo, dest, lengthOfCE, numCharsReadAlready);
            
            readSeekSet(volInfo, origPos, SEEK_SET);
            
            return rc;
        }
    }
    else
        return 1;
}

/* if no SL record is found does not return failure */
int readRockridgeSymlink(VolInfo* volInfo, BkSymLink** dest, int lenSU)
{
    bk_off_t origPos;
    unsigned char* suFields;
    int rc;
    int count;
    int count2;
    
    suFields = malloc(lenSU);
    if(suFields == NULL)
        return BKERROR_OUT_OF_MEMORY;
    
    origPos = readSeekTell(volInfo);
    
    rc = readRead(volInfo, suFields, lenSU);
    if(rc != lenSU)
    {
        free(suFields);
        return BKERROR_READ_GENERIC;
    }
    
    count = 0;
    while(count < lenSU)
    {
        if(suFields[count] == 0)
        /* not an SU field, mkisofs sometimes has a trailing 0 in the directory
        * record and this is the easiest way to ignore it */
            break;
        
        if(suFields[count] == 'S' && suFields[count + 1] == 'L')
        {
            size_t numCharsUsed; /* in dest->target, not including '\0' */
            
            *dest = malloc(sizeof(BkSymLink));
            if(*dest == NULL)
                return BKERROR_OUT_OF_MEMORY;
            
            memset(*dest, 0, sizeof(BkSymLink));
            
            numCharsUsed = 0;
            (*dest)->target[0] = '\0';
            /* read sym link component records and assemble (*dest)->target
            * right now component records cannot spawn multiple SL entries */
            count2 = count + 5;
            while(count2 < count + suFields[count + 2])
            {
                if(suFields[count2] & 0x02)
                {
                    numCharsUsed += appendStringIfHaveRoom((*dest)->target, 
                                            ".", NCHARS_SYMLINK_TARGET_MAX - 1, 
                                            numCharsUsed, -1);
                }
                else if(suFields[count2] & 0x04)
                {
                    numCharsUsed += appendStringIfHaveRoom((*dest)->target, 
                                            "..", NCHARS_SYMLINK_TARGET_MAX - 1, 
                                            numCharsUsed, -1);
                }
                else if(suFields[count2] & 0x08)
                {
                    strcpy((*dest)->target, "/");
                    numCharsUsed = 1;
                }
                
                /* if bits 1-5 are set there is no component content */
                if( !(suFields[count2] & 0x3E) )
                {
                    numCharsUsed += appendStringIfHaveRoom((*dest)->target, 
                                            (char*)(suFields + count2 + 2), 
                                            NCHARS_SYMLINK_TARGET_MAX - 1, 
                                            numCharsUsed, suFields[count2 + 1]);
                }
                
                /* next component record */
                count2 += suFields[count2 + 1] + 2;
                
                if(count2 < count + suFields[count + 2])
                /* another component record follows, insert separator */
                {
                    numCharsUsed += appendStringIfHaveRoom((*dest)->target, 
                                            "/", NCHARS_SYMLINK_TARGET_MAX - 1, 
                                            numCharsUsed, -1);
                }
            }
            
            /* ignore any other SU fields */
            break;
        }
        
        /* skip su field */
        count += suFields[count + 2];
    }
    
    free(suFields);
    readSeekSet(volInfo, origPos, SEEK_SET);
    
    return 1;
}

/*******************************************************************************
* removeCrapFromFilename()
* filenames as read from 9660 Sometimes end with ;1 (terminator+version num)
* this removes the useless ending and terminates the destination with a '\0'
* */
void removeCrapFromFilename(char* filename, int length)
{
    int count;
    bool stop = false;
    
    for(count = 0; count < length && !stop; count++)
    {
        if(filename[count] == ';')
        {
            filename[count] = '\0';
            stop = true;
        }
    }
    
    /* if did not get a ';' terminate string anyway */
    filename[count] = '\0';
}

/*******************************************************************************
* skipDR()
* Seek past a directory entry. Good for skipping "." and ".."
* */
int skipDR(VolInfo* volInfo)
{
    unsigned char dRLen;
    int rc;
    
    rc = read711(volInfo, &dRLen);
    if(rc <= 0)
        return BKERROR_READ_GENERIC;
    
    readSeekSet(volInfo, dRLen - 1, SEEK_CUR);
    
    return dRLen;
}

/*******************************************************************************
* stripSpacesFromEndOfString
* Some strings in the ISO volume are padded with spaces (hopefully on the right)
* this function removes them.
* */
void stripSpacesFromEndOfString(char* str)
{
    size_t count;
    
    for(count = strlen(str) - 1; str[count] == ' '; count--)
    {
        str[count] = '\0';

        if(count == 0) /* unsigned */
            break;
    }
}
