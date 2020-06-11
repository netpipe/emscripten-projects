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

#include "SFN.h"
#include "__private/SFN.h"
#include "Display.h"

char * SFNCreate( DiskRef o, const char * path )
{
    char * file;
    char * ext;
    char * name;
    char * fullname;
    size_t len;
    
    if( o == NULL || path == NULL || strlen( path ) == 0 )
    {
        return NULL;
    }
    
    name     = malloc( 12 );
    fullname = malloc( strlen( path ) + 1 );
    
    if( name == NULL || fullname == NULL )
    {
        DisplayPrintError( "Out of memory" );
        
        return NULL;
    }
    
    name[ 11 ] = 0;
    
    memset( name, ' ', 11 );
    strcpy( fullname, path );
    
    file = strrchr( fullname, '/' );
    
    if( file == NULL )
    {
        file = fullname;
    }
    else
    {
        file++;
    }
    
    ext = strrchr( fullname, '.' );
    
    if( ext != NULL )
    {
        ext[ 0 ] = 0;
        
        ext++;
    }
    
    __SFNReplaceInvalidCharacters( o, file );
    
    if( ext != NULL )
    {
        __SFNReplaceInvalidCharacters( o, ext );
        
        len = strlen( ext );
        
        memcpy( name + 8, ext, ( len > 3 ) ? 3 : len );
    }
    
    len = strlen( file );
    
    if( len > 8 )
    {
        memcpy( name, file, 6 );
        
        name[ 6 ] = '~';
        name[ 7 ] = '1';
    }
    else
    {
        memcpy( name, file, len );
    }
    
    free( fullname );
    
    if( __SFNUniqueFilename( o, name ) == false )
    {
        free( name );
        
        return NULL;
    }
    
    return name;
}
