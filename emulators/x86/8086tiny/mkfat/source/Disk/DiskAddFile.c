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
#include "IO.h"
#include "Arguments.h"
#include "SFN.h"

#define DISK_FILE_BUFFER_SIZE  10

bool DiskAddFile( MutableDiskRef o, const char * file )
{
    char * path;
    char * name;
    size_t size;
    
    if( o == NULL || file == NULL || strlen( file ) == 0 )
    {
        return false;
    }
    
    path = malloc( strlen( file ) + 1 );
    
    if( path == NULL )
    {
        DisplayPrintError( "Out of memory" );
        
        return false;
    }
    
    strcpy( path, file );
    
    name = SFNCreate( o, path );
    
    if( name == NULL )
    {
        DisplayPrintError( "Cannot create a FAT filename for file: %s", file );
        
        free( path );
        
        return false;
    }
    
    if( o->fileBufferSize == 0 )
    {
        o->filePaths       = calloc( sizeof( const char * ), DISK_FILE_BUFFER_SIZE );
        o->filenames       = calloc( sizeof( const char * ), DISK_FILE_BUFFER_SIZE );
        o->fileSizes       = calloc( sizeof( size_t       ), DISK_FILE_BUFFER_SIZE );
        o->fileBufferSize  = DISK_FILE_BUFFER_SIZE;
    }
    else if( o->fileCount == o->fileBufferSize )
    {
        o->filePaths       = realloc( o->filePaths, sizeof( const char * ) * ( o->fileBufferSize + DISK_FILE_BUFFER_SIZE ) );
        o->filenames       = realloc( o->filenames, sizeof( const char * ) * ( o->fileBufferSize + DISK_FILE_BUFFER_SIZE ) );
        o->fileSizes       = realloc( o->fileSizes, sizeof( size_t       ) * ( o->fileBufferSize + DISK_FILE_BUFFER_SIZE ) );
        o->fileBufferSize += DISK_FILE_BUFFER_SIZE;
    }
    
    if( o->filePaths == NULL || o->filenames == NULL || o->fileSizes == NULL )
    {
        o->fileCount      = 0;
        o->fileBufferSize = 0;
        
        free( o->filePaths );
        free( o->filenames );
        free( o->fileSizes );
        free( path );
        free( name );
        
        DisplayPrintError( "Out of memory" );
        
        return false;
    }
    
    size = IOGetFileSize( path );
    
    if( ArgumentsGetVerbose( ArgumentsGetCurrent() ) )
    {
        printf
        (
            "Adding file #%zu\n"
            "    - Local path: %s\n"
            "    - FAT name:   <%s>\n"
            "    - File size:  %zu bytes\n",
            o->fileCount + 1,
            path,
            name,
            size
        );
    }
    
    o->filePaths[ o->fileCount ] = path;
    o->filenames[ o->fileCount ] = name;
    o->fileSizes[ o->fileCount ] = size;
    
    o->fileCount++;
    
    return true;
}
