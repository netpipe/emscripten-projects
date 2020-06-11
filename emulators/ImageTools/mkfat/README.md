mkfat
=====

[![Build Status](https://img.shields.io/travis/macmade/mkfat.svg?branch=master&style=flat)](https://travis-ci.org/macmade/mkfat)
[![Issues](http://img.shields.io/github/issues/macmade/mkfat.svg?style=flat)](https://github.com/macmade/mkfat/issues)
![Status](https://img.shields.io/badge/status-active-brightgreen.svg?style=flat)
![License](https://img.shields.io/badge/license-bsd-brightgreen.svg?style=flat)
[![Contact](https://img.shields.io/badge/contact-@macmade-blue.svg?style=flat)](https://twitter.com/macmade)  
[![Donate-Patreon](https://img.shields.io/badge/donate-patreon-yellow.svg?style=flat)](https://patreon.com/macmade)
[![Donate-Gratipay](https://img.shields.io/badge/donate-gratipay-yellow.svg?style=flat)](https://www.gratipay.com/macmade)
[![Donate-Paypal](https://img.shields.io/badge/donate-paypal-yellow.svg?style=flat)](https://paypal.me/xslabs)

About
-----

mkfat is a command line tool to create FAT disk images.  
Supported formats are FAT-12 and FAT-16.

Usage
-----

    Usage:
        
        mkfat [options] -o <disk> [<file>...]
        
    Options:
    
        --ss           uint        Sector size
        --sc           uint        Sectors per cluster
        --rsc          uint        Reserved sector count
        --fn           uint        Number of FATs
        --rde          uint        Number of root directory entries
        --ts           uint        Total sectors
        --sf           uint        Sectors per FAT
        --spt          uint        Sectors per track
        --nos          uint        Number of sides
        --medium       uint        Medium identifier
        --signature    uint        Extended boot record signature
        --id           uint        Volume ID number
        --label        string      Volume label (up to 11 characters)
        --format       string      File system type [FAT12/FAT16]
        --creator      string      Creating system identifier (up to 8 characters)
        --bootable                 Makes the disk image bootable
        --help, -h                 Prints this help message
        --verbose, -v              Verbose mode

License
-------

mkfat is released under the terms of the BSD License.

Repository Infos
----------------

    Owner:			Jean-David Gadina - XS-Labs
    Web:			www.xs-labs.com
    Blog:			www.noxeos.com
    Twitter:		@macmade
    GitHub:			github.com/macmade
    LinkedIn:		ch.linkedin.com/in/macmade/
    StackOverflow:	stackoverflow.com/users/182676/macmade
