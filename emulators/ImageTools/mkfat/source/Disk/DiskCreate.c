/*******************************************************************************
 * Copyright (c) 2015, Jean-David Gadina - www.xs-labs.com
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  -   Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *  -   Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *  -   Neither the name of 'Jean-David Gadina' nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*!
 * @author          Jean-David Gadina
 * @copyright       (c) 2015, Jean-David Gadina - www.xs-labs.com
 */

#include "Disk.h"
#include "__private/Disk.h"
#include "Display.h"

MutableDiskRef DiskCreate( DiskFormat format )
{
    struct __Disk * o;
    MutableMBRRef   mbr;
    
    o = calloc( sizeof( struct __Disk ), 1 );
    
    if( o == NULL )
    {
        DisplayPrintError( "Out of memory" );
        
        return NULL;
    }
    
    mbr = MBRCreate( o );
    
    if( mbr == NULL )
    {
        return NULL;
    }
    
    o->mbr = mbr;
    
    if( format == DiskFormatFAT12 )
    {
        MBRSetSectorSize(                   mbr, 512 );
        MBRSetSectorsPerCluster(            mbr, 0 );
        MBRSetReservedSectorCount(          mbr, 1 );
        MBRSetNumberOfFATs(                 mbr, 2 );
        MBRSetNumberOfRootDirectoryEntries( mbr, 512 );
        MBRSetTotalSectors(                 mbr, 0 );
        MBRSetMediumIdentifier(             mbr, 0xF8 );
        MBRSetSectorsPerFAT(                mbr, 0 );
        MBRSetSectorsPerTrack(              mbr, 32 );
        MBRSetNumberOfSides(                mbr, 16 );
        MBRSetExtendedBootRecordSignature(  mbr, 0x29 );
        MBRSetVolumeIDNumber(               mbr, ( uint32_t )time( NULL ) );
        MBRSetVolumeLabel(                  mbr, "UNTITLED" );
        MBRSetFileSystemType(               mbr, "FAT12" );
        MBRSetCreatingSystemIdentifier(     mbr, "MKFAT" );
        MBRSetBootable(                     mbr, false );
        
        /*
        - OEM ID:                  BSD  4.4
        - Bytes per sector:        512
        - Sectors per cluster:     1
        - Reserved sectors:        1
        - Number of FATs:          2
        - Max root dir entries:    512
        - Total sectors:           2879
        - Media descriptor:        0xF8
        - Sectors per FAT:         9
        - Sectors per track:       32
        - Heads per cylinder:      16
        - Hidden sectors:          1
        - LBA sectors:             0
        - Drive number:            0x80
        - Extended boot signature: 0x29
        - Volume serial number:    0x821519FA
        - Volume label:            TEST       
        - File system:             FAT12   
        - Bootable:                yes
        */
    }
    else
    {
        MBRSetSectorSize(                   mbr, 512 );
        MBRSetSectorsPerCluster(            mbr, 0 );
        MBRSetReservedSectorCount(          mbr, 1 );
        MBRSetNumberOfFATs(                 mbr, 2 );
        MBRSetNumberOfRootDirectoryEntries( mbr, 512 );
        MBRSetTotalSectors(                 mbr, 0 );
        MBRSetMediumIdentifier(             mbr, 0xF8 );
        MBRSetSectorsPerFAT(                mbr, 0 );
        MBRSetSectorsPerTrack(              mbr, 32 );
        MBRSetNumberOfSides(                mbr, 16 );
        MBRSetExtendedBootRecordSignature(  mbr, 0x29 );
        MBRSetVolumeIDNumber(               mbr, ( uint32_t )time( NULL ) );
        MBRSetVolumeLabel(                  mbr, "UNTITLED" );
        MBRSetFileSystemType(               mbr, "FAT16" );
        MBRSetCreatingSystemIdentifier(     mbr, "MKFAT" );
        MBRSetBootable(                     mbr, false );
        
        /*
        - OEM ID:                  BSD  4.4
        - Bytes per sector:        512
        - Sectors per cluster:     2
        - Reserved sectors:        1
        - Number of FATs:          2
        - Max root dir entries:    512
        - Total sectors:           28799
        - Media descriptor:        0xF8
        - Sectors per FAT:         56
        - Sectors per track:       32
        - Heads per cylinder:      16
        - Hidden sectors:          1
        - LBA sectors:             0
        - Drive number:            0x80
        - Extended boot signature: 0x29
        - Volume serial number:    0x42F419FA
        - Volume label:            TEST       
        - File system:             FAT16   
        - Bootable:                yes
        */
    }
    
    return o;
}
