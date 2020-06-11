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

#include "DirectoryEntry.h"
#include "__private/DirectoryEntry.h"
#include "IO.h"
#include "Display.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool DirectoryEntrySetInfoFromLocalFile( DirectoryEntryRef o, const char * path )
{
    struct stat info;
    
    if( o == NULL || path == NULL || strlen( path ) == 0 )
    {
        return false;
    }
    
    if( IOFileExists( path ) == false )
    {
        DisplayPrintError( "File does not exist: %s", path );
        
        return false;
    }
    
    if( stat( path, &info ) != 0 )
    {
        DisplayPrintError( "Cannot get info from file: %s", path );
        
        return false;
    }
    
    if( S_ISREG( info.st_mode ) == 0 )
    {
        DisplayPrintError( "Only regulare files are supported: %s", path );
        
        return false;
    }
    
    o->entry->fileLength            = ( uint32_t )IOGetFileSize( path );
    o->entry->attributes            = DirectoryEntryAttributeArchive;
    o->entry->creationTime          = __DirectoryEntryTimeFromTS( info.st_ctime );
    o->entry->creationDate          = __DirectoryEntryDateFromTS( info.st_ctime );
    o->entry->lastAccessDate        = __DirectoryEntryDateFromTS( info.st_atime );
    o->entry->lastModificationTime  = __DirectoryEntryTimeFromTS( info.st_mtime );
    o->entry->lastModificationDate  = __DirectoryEntryDateFromTS( info.st_mtime );
    
    return true;
}
