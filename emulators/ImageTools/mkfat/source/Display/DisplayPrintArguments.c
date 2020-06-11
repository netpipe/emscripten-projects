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

#include "Display.h"

void DisplayPrintArguments( ArgumentsRef args )
{
    size_t i;
    
    if( args == NULL )
    {
        return;
    }
    
    DisplayPrintHeader( "Arguments:" );
    
    printf
    (
        "Sector size:                       %lu\n"
        "Sectors per cluster:               %lu\n"
        "Reserved sector count:             %lu\n"
        "Number of FATs:                    %lu\n"
        "Number of root directory entries:  %lu\n"
        "Total sectors:                     %lu\n"
        "Sectors per FAT:                   %lu\n"
        "Sectors per track:                 %lu\n"
        "Number of sides:                   %lu\n"
        "Medium identifier:                 0x%lX\n"
        "Extended boot record signature:    0x%lX\n"
        "Volume ID number:                  0x%lX\n"
        "Volume label:                      %s\n"
        "File system type:                  %s\n"
        "Creating system identifier:        %s\n"
        "Bootable:                          %s\n"
        "Help:                              %s\n"
        "Verbose:                           %s\n"
        "Output path:                       %s\n"
        "Input files:                       %lu\n",
        ( unsigned long )ArgumentsGetSectorSize( args ),
        ( unsigned long )ArgumentsGetSectorsPerCluster( args ),
        ( unsigned long )ArgumentsGetReservedSectorCount( args ),
        ( unsigned long )ArgumentsGetNumberOfFATs( args ),
        ( unsigned long )ArgumentsGetNumberOfRootDirectoryEntries( args ),
        ( unsigned long )ArgumentsGetTotalSectors( args ),
        ( unsigned long )ArgumentsGetSectorsPerFAT( args ),
        ( unsigned long )ArgumentsGetSectorsPerTrack( args ),
        ( unsigned long )ArgumentsGetNumberOfSides( args ),
        ( unsigned long )ArgumentsGetMediumIdentifier( args ),
        ( unsigned long )ArgumentsGetExtendedBootRecordSignature( args ),
        ( unsigned long )ArgumentsGetVolumeIDNumber( args ),
        ( ArgumentsGetVolumeLabel( args ) )              ? ArgumentsGetVolumeLabel( args )              : "-",
        ( ArgumentsGetFileSystemType( args ) )           ? ArgumentsGetFileSystemType( args )           : "-",
        ( ArgumentsGetCreatingSystemIdentifier( args ) ) ? ArgumentsGetCreatingSystemIdentifier( args ) : "-",
        ( ArgumentsGetBootable( args ) )                 ? "yes"                                        : "no",
        ( ArgumentsGetShowHelp( args ) )                 ? "yes"                                        : "no",
        ( ArgumentsGetVerbose( args ) )                  ? "yes"                                        : "no",
        ( ArgumentsGetDiskPath( args ) )                 ? ArgumentsGetDiskPath( args )                 : "-",
        ( unsigned long )ArgumentsGetInputFileCount( args )
    );
    
    for( i = 0; i < ArgumentsGetInputFileCount( args ); i++ )
    {
        printf( "    - %s\n", ArgumentsGetInputFileAtIndex( args, i ) );
    }
}
