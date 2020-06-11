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

#ifndef MKFAT_MBR_H
#define MKFAT_MBR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef const struct __MBR * MBRRef;
typedef       struct __MBR * MutableMBRRef;

#include "C99.h"
#include "Disk.h"

MutableMBRRef   MBRCreate( DiskRef disk );
void            MBRDelete( MutableMBRRef o );

DiskRef MBRGetDisk( MBRRef o );
bool    MBRWrite( MBRRef o, FILE * fp );

uint16_t      MBRGetSectorSize( MBRRef o );
uint8_t       MBRGetSectorsPerCluster( MBRRef o );
uint16_t      MBRGetReservedSectorCount( MBRRef o );
uint8_t       MBRGetNumberOfFATs( MBRRef o );
uint16_t      MBRGetNumberOfRootDirectoryEntries( MBRRef o );
uint16_t      MBRGetTotalSectors( MBRRef o );
uint8_t       MBRGetMediumIdentifier( MBRRef o );
uint16_t      MBRGetSectorsPerFAT( MBRRef o );
uint16_t      MBRGetSectorsPerTrack( MBRRef o );
uint16_t      MBRGetNumberOfSides( MBRRef o );
uint8_t       MBRGetExtendedBootRecordSignature( MBRRef o );
uint32_t      MBRGetVolumeIDNumber( MBRRef o );
const char  * MBRGetVolumeLabel( MBRRef o );
const char  * MBRGetFileSystemType( MBRRef o );
const char  * MBRGetCreatingSystemIdentifier( MBRRef o );
bool          MBRIsBootable( MBRRef o );

void MBRSetSectorSize( MBRRef o, uint16_t value );
void MBRSetSectorsPerCluster( MBRRef o, uint8_t value );
void MBRSetReservedSectorCount( MBRRef o, uint16_t value );
void MBRSetNumberOfFATs( MBRRef o, uint8_t value );
void MBRSetNumberOfRootDirectoryEntries( MBRRef o, uint16_t value );
void MBRSetTotalSectors( MBRRef o, uint16_t value );
void MBRSetMediumIdentifier( MBRRef o, uint8_t value );
void MBRSetSectorsPerFAT( MBRRef o, uint16_t value );
void MBRSetSectorsPerTrack( MBRRef o, uint16_t value );
void MBRSetNumberOfSides( MBRRef o, uint16_t value );
void MBRSetExtendedBootRecordSignature( MBRRef o, uint8_t value );
void MBRSetVolumeIDNumber( MBRRef o, uint32_t value );
void MBRSetVolumeLabel( MBRRef o, const char * value );
void MBRSetFileSystemType( MBRRef o, const char * value );
void MBRSetCreatingSystemIdentifier( MBRRef o, const char * value );
void MBRSetBootable( MBRRef o, bool value );

#ifdef __cplusplus
}
#endif

#endif /* MKFAT_MBR_H */
