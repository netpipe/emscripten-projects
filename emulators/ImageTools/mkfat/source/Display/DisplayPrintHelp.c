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

void DisplayPrintHelp( void )
{
    printf
    (
        "Usage:\n"
        "\n"
        "    mkfat [options] -o <disk> [<file>...]\n"
        "\n"
        "Options:\n"
        "\n"
        "    --ss           uint        Sector size\n"
        "    --sc           uint        Sectors per cluster\n"
        "    --rsc          uint        Reserved sector count\n"
        "    --fn           uint        Number of FATs\n"
        "    --rde          uint        Number of root directory entries\n"
        "    --ts           uint        Total sectors\n"
        "    --sf           uint        Sectors per FAT\n"
        "    --spt          uint        Sectors per track\n"
        "    --nos          uint        Number of sides\n"
        "    --medium       uint        Medium identifier\n"
        "    --signature    uint        Extended boot record signature\n"
        "    --id           uint        Volume ID number\n"
        "    --label        string      Volume label (up to 11 characters)\n"
        "    --format       string      File system type [FAT12/FAT16]\n"
        "    --creator      string      Creating system identifier (up to 8 characters)\n"
        "    --bootable                 Makes the disk image bootable\n"
        "    --help, -h                 Prints this help message\n"
        "    --verbose, -v              Verbose mode\n"
    );
}
