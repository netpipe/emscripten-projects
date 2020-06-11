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

#ifndef MKFAT_DIRECTORY_ENTRY_H
#define MKFAT_DIRECTORY_ENTRY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct __DirectoryEntry * DirectoryEntryRef;
typedef       struct __DirectoryEntry * MutableDirectoryEntryRef;

#include "C99.h"
#include "Directory.h"

typedef enum
{
    DirectoryEntryAttributeReadOnly     = 0x01,
    DirectoryEntryAttributeHidden       = 0x02,
    DirectoryEntryAttributeSystem       = 0x04,
    DirectoryEntryAttributeVolumeLabel  = 0x08,
    DirectoryEntryAttributeDirectory    = 0x10,
    DirectoryEntryAttributeArchive      = 0x20,
    DirectoryEntryAttributeLFN          = 0x0F
}
DirectoryEntryAttributes;

MutableDirectoryEntryRef DirectoryEntryCreate( DirectoryRef directory );
void                     DirectoryEntryDelete( MutableDirectoryEntryRef o );

DirectoryRef DirectoryEntryGetDirectory( DirectoryEntryRef o );
bool         DirectoryEntrySetInfoFromLocalFile( DirectoryEntryRef o, const char * path );
bool         DirectoryEntryWrite( DirectoryEntryRef o, FILE * fp );

const char * DirectoryEntryGetFilename( DirectoryEntryRef o );
uint8_t      DirectoryEntryGetAttributes( DirectoryEntryRef o );
uint16_t     DirectoryEntryGetCreationTime( DirectoryEntryRef o );
uint16_t     DirectoryEntryGetCreationDate( DirectoryEntryRef o );
uint16_t     DirectoryEntryGetLastAccessDate( DirectoryEntryRef o );
uint16_t     DirectoryEntryGetLastModificationTime( DirectoryEntryRef o );
uint16_t     DirectoryEntryGetLastModificationDate( DirectoryEntryRef o );
uint16_t     DirectoryEntryGetStartingClusterNumber( DirectoryEntryRef o );
uint32_t     DirectoryEntryGetFileLength( DirectoryEntryRef o );

void DirectoryEntrySetFilename( DirectoryEntryRef o, const char * value );
void DirectoryEntrySetAttributes( DirectoryEntryRef o, uint8_t value );
void DirectoryEntrySetCreationTime( DirectoryEntryRef o, uint16_t value );
void DirectoryEntrySetCreationDate( DirectoryEntryRef o, uint16_t value );
void DirectoryEntrySetLastAccessDate( DirectoryEntryRef o, uint16_t value );
void DirectoryEntrySetLastModificationTime( DirectoryEntryRef o, uint16_t value );
void DirectoryEntrySetLastModificationDate( DirectoryEntryRef o, uint16_t value );
void DirectoryEntrySetStartingClusterNumber( DirectoryEntryRef o, uint16_t value );
void DirectoryEntrySetFileLength( DirectoryEntryRef o, uint32_t value );

#ifdef __cplusplus
}
#endif

#endif /* MKFAT_DIRECTORY_ENTRY_H */
