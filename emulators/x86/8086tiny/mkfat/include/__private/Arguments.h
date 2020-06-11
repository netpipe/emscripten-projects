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

#ifndef MKFAT___PRIVATE_ARGUMENTS_H
#define MKFAT___PRIVATE_ARGUMENTS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../Arguments.h"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif

struct __Arguments
{
    size_t        sectorSize;
    size_t        sectorsPerCluster;
    size_t        reservedSectorCount;
    size_t        numberOfFATs;
    size_t        numberOfRootDirectoryEntries;
    size_t        totalSectors;
    size_t        sectorsPerFAT;
    size_t        sectorsPerTrack;
    size_t        numberOfSides;
    unsigned int  mediumIdentifier;
    unsigned int  extendedBootRecordSignature;
    unsigned int  volumeIDNumber;
    const char  * volumeLabel;
    const char  * fileSystemType;
    const char  * creatingSystemIdentifier;
    bool          bootable;
    bool          showHelp;
    bool          verbose;
    const char  * diskPath;
    const char ** inputFiles;
    size_t        inputFileCount;
    size_t        inputFileBufferSize;
};

extern ArgumentsRef __ArgumentsCurrent;

bool __ArgumentsAddFile( struct __Arguments * o, const char * file );

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#ifdef __cplusplus
}
#endif

#endif /* MKFAT___PRIVATE_ARGUMENTS_H */
