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

bool __SFNUniqueFilename( DiskRef o, char * filename )
{
    size_t i;
    size_t n;
    size_t c;
    char   no[ 2 ];
    char * tilde;
    
    if( o == NULL || filename == NULL || strlen( filename ) == 0 )
    {
        return false;
    }
    
    c     = 0;
    tilde = strchr( filename, '~' );
    
    if( tilde != NULL )
    {
        no[ 0 ] = tilde[ 1 ];
        no[ 1 ] = 0;
        c       = ( size_t )strtoul( no, NULL, 0 );
    }
    
    find:
    
    n = DiskGetFileCount( o );
    
    for( i = 0; i < n; i++ )
    {
        if( strcmp( filename, DiskGetFilenameAtIndex( o, i ) ) == 0 )
        {
            if( tilde == NULL )
            {
                for( i = 0; i < 7; i++ )
                {
                    if( filename[ i ] == ' ' )
                    {
                        tilde             = filename + i;
                        filename[ i     ] = '~';
                        filename[ i + 1 ] = '1';
                        
                        goto find;
                    }
                }
                
                tilde         = filename + 6;
                filename[ 6 ] = '~';
                filename[ 7 ] = '1';
                
                goto find;
            }
            else if( c < 10 )
            {
                tilde[ 1 ] = ( char )( ++c + 48 );
            }
            else
            {
                return false;
            }
            
            goto find;
        }
    }
    
    return true;
}
