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

#include "C99.h"
#include "Arguments.h"
#include "Display.h"
#include "Disk.h"

int main( int argc, char * argv[] )
{
    int                 status;
    MutableArgumentsRef args;
    MutableDiskRef      disk;
    MutableMBRRef       mbr;
    size_t              i;
    
    args = ArgumentsCreate( argc, argv );
    disk = NULL;
    
    if( ArgumentsGetVerbose( args ) )
    {
        DisplayPrintArguments( args );
    }
    
    if( ArgumentsGetShowHelp( args ) )
    {
        DisplayPrintHelp();
        
        goto success;
    }
    
    if( ArgumentsValidate( args ) == false )
    {
        goto failure;
    }
    
    if
    (
           ArgumentsGetFileSystemType( args )                    == NULL
        || strcmp( ArgumentsGetFileSystemType( args ), "FAT16" ) == 0
        || strcmp( ArgumentsGetFileSystemType( args ), "fat16" ) == 0
    )
    {
        disk = DiskCreate( DiskFormatFAT16 );
    }
    else
    {
        disk = DiskCreate( DiskFormatFAT12 );
    }
    
    if( disk == NULL )
    {
        goto failure;
    }
    
    mbr = DiskGetMBR( disk );
    
    if( ArgumentsGetSectorSize( args ) )
    {
        MBRSetSectorSize( mbr, ( uint16_t )ArgumentsGetSectorSize( args ) );
    }
    
    if( ArgumentsGetSectorsPerCluster( args ) )
    {
        MBRSetSectorsPerCluster( mbr, ( uint8_t )ArgumentsGetSectorsPerCluster( args ) );
    }
    
    if( ArgumentsGetReservedSectorCount( args ) )
    {
        MBRSetReservedSectorCount( mbr, ( uint16_t )ArgumentsGetReservedSectorCount( args ) );
    }
    
    if( ArgumentsGetNumberOfFATs( args ) )
    {
        MBRSetNumberOfFATs( mbr, ( uint8_t )ArgumentsGetNumberOfFATs( args ) );
    }
    
    if( ArgumentsGetNumberOfRootDirectoryEntries( args ) )
    {
        MBRSetNumberOfRootDirectoryEntries( mbr, ( uint16_t )ArgumentsGetNumberOfRootDirectoryEntries( args ) );
    }
    
    if( ArgumentsGetTotalSectors( args ) )
    {
        MBRSetTotalSectors( mbr, ( uint16_t )ArgumentsGetTotalSectors( args ) );
    }
    
    if( ArgumentsGetSectorsPerFAT( args ) )
    {
        MBRSetSectorsPerFAT( mbr, ( uint16_t )ArgumentsGetSectorsPerFAT( args ) );
    }
    
    if( ArgumentsGetSectorsPerTrack( args ) )
    {
        MBRSetSectorsPerTrack( mbr, ( uint16_t )ArgumentsGetSectorsPerTrack( args ) );
    }
    
    if( ArgumentsGetNumberOfSides( args ) )
    {
        MBRSetNumberOfSides( mbr, ( uint16_t )ArgumentsGetNumberOfSides( args ) );
    }
    
    if( ArgumentsGetMediumIdentifier( args ) )
    {
        MBRSetMediumIdentifier( mbr, ( uint8_t )ArgumentsGetMediumIdentifier( args ) );
    }
    
    if( ArgumentsGetExtendedBootRecordSignature( args ) )
    {
        MBRSetExtendedBootRecordSignature( mbr, ( uint8_t )ArgumentsGetExtendedBootRecordSignature( args ) );
    }
    
    if( ArgumentsGetVolumeIDNumber( args ) )
    {
        MBRSetVolumeIDNumber( mbr, ( uint32_t )ArgumentsGetVolumeIDNumber( args ) );
    }
    
    if( ArgumentsGetVolumeLabel( args ) )
    {
        MBRSetVolumeLabel( mbr, ArgumentsGetVolumeLabel( args ) );
    }
    
    if( ArgumentsGetCreatingSystemIdentifier( args ) )
    {
        MBRSetCreatingSystemIdentifier( mbr, ArgumentsGetCreatingSystemIdentifier( args ) );
    }
    
    if( ArgumentsGetBootable( args ) )
    {
        MBRSetBootable( mbr, true );
    }
    else
    {
        MBRSetBootable( mbr, false );
    }
    
    if( ArgumentsGetVerbose( args ) && ArgumentsGetInputFileCount( args ) )
    {
        DisplayPrintHeader( "Adding files to disk:" );
    }
    
    for( i = 0; i < ArgumentsGetInputFileCount( args ); i++ )
    {
        if( DiskAddFile( disk, ArgumentsGetInputFileAtIndex( args, i ) ) == false )
        {
            goto failure;
        }
    }
    
    if( DiskWrite( disk, ArgumentsGetDiskPath( args ) ) == false )
    {
        goto failure;
    }
        
    success:
        
        status = EXIT_SUCCESS;
        
        goto cleanup;
        
    failure:
        
        status = EXIT_FAILURE;
    
    cleanup:
        
        DiskDelete( disk );
        ArgumentsDelete( args );
    
    return status;
}

