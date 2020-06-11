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

#include "Arguments.h"
#include "__private/Arguments.h"
#include "Display.h"

MutableArgumentsRef ArgumentsCreate( int argc, char ** argv )
{
    struct __Arguments * o;
    int                  i;
    
    o = calloc( sizeof( struct __Arguments ), 1 );
    
    if( o == NULL )
    {
        DisplayPrintError( "Out of memory" );
        
        return NULL;
    }
    
    for( i = 1; i < argc; i++ )
    {
        if( strcmp( argv[ i ], "--ss" ) == 0 && i + 1 < argc )
        {
            o->sectorSize = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--sc" ) == 0 && i + 1 < argc )
        {
            o->sectorsPerCluster = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--rsc" ) == 0 && i + 1 < argc )
        {
            o->reservedSectorCount = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--fn" ) == 0 && i + 1 < argc )
        {
            o->numberOfFATs = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--rde" ) == 0 && i + 1 < argc )
        {
            o->numberOfRootDirectoryEntries = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--ts" ) == 0 && i + 1 < argc )
        {
            o->totalSectors = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--sf" ) == 0 && i + 1 < argc )
        {
            o->sectorsPerFAT = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--spt" ) == 0 && i + 1 < argc )
        {
            o->sectorsPerTrack = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--nos" ) == 0 && i + 1 < argc )
        {
            o->numberOfSides = ( size_t )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--medium" ) == 0 && i + 1 < argc )
        {
            o->mediumIdentifier = ( unsigned int )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--signature" ) == 0 && i + 1 < argc )
        {
            o->extendedBootRecordSignature = ( unsigned int )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--id" ) == 0 && i + 1 < argc )
        {
            o->volumeIDNumber = ( unsigned int )strtoul( argv[ ++i ], NULL, 0 );
        }
        else if( strcmp( argv[ i ], "--label" ) == 0 && i + 1 < argc )
        {
            o->volumeLabel = argv[ ++i ];
        }
        else if( strcmp( argv[ i ], "--format" ) == 0 && i + 1 < argc )
        {
            o->fileSystemType = argv[ ++i ];
        }
        else if( strcmp( argv[ i ], "--creator" ) == 0 && i + 1 < argc )
        {
            o->creatingSystemIdentifier = argv[ ++i ];
        }
        else if( strcmp( argv[ i ], "--bootable" ) == 0 )
        {
            o->bootable = true;
        }
        else if( strcmp( argv[ i ], "--help" ) == 0 )
        {
            o->showHelp = true;
        }
        else if( strcmp( argv[ i ], "--verbose" ) == 0 )
        {
            o->verbose = true;
        }
        else if( strcmp( argv[ i ], "-h" ) == 0 )
        {
            o->showHelp = true;
        }
        else if( strcmp( argv[ i ], "-v" ) == 0 )
        {
            o->verbose = true;
        }
        else if( strcmp( argv[ i ], "-o" ) == 0 && i + 1 < argc )
        {
            o->diskPath = argv[ ++i ];
        }
        else if( __ArgumentsAddFile( o, argv[ i ] ) == false )
        {
            free( o );
            
            return NULL;
        }
    }
    
    __ArgumentsCurrent = o;
    
    return o;
}
