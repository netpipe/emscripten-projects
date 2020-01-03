ISO Master
==========
http://littlesvr.ca/isomaster/

ISO Master is an open-source, easy to use, graphical CD image editor for Linux and BSD. Basically you can use this program to extract files from an ISO, add files to an ISO, and create bootable ISOs - all in a graphical user interface. It can open ISO, NRG, and some MDF files but can only save as ISO.

ISO Master is based on bkisofs - a library for reading, modifying and writing ISO images.

Prerequisites for Compiling from Source
---------------------------------------
You need a C compiler, make and whatever packages required for compiling GTK2 applications. On ubuntu I had to install (using Synaptic): build-essential, libgtk2.0-dev, pkg-config and gettext. On Slackware I didn't have to install anything.

Compiling and Installing from Source
------------------------------------
At this time bkisofs comes in the same package as ISO Master so it doesn't need to be downloaded/built separately.

To compile, simply type

make

at the command prompt in the directory where you extracted the archive.

And to install (this is optional, you can run from the directory where you compiled it, though you won't get icons if you don't install):

su
[enter root password]
make install

Or on something like Ubuntu that won't let you su:

sudo make install
[enter user's password]

If you get errors, or even warnings, please do send me the output so I can fix them.

Uninstalling from Source
------------------------
If you followed the steps above to install ISO Master, use the following to uninstall it:

su
[enter root password]
make uninstall

Or on something like Ubuntu that won't let you su:

sudo make uninstall
[enter user's password]

Contact
-------
I would love to hear from you, to know how you made use of either ISO Master or bkisofs, or with bug reports, or feature requests. Please use the form at http://littlesvr.ca/misc/contactandrew.php to send me email. Thanks in advance!

Bug Reports
-----------
Please help me improve ISO Master by sending me bug reports. The more of the following you can do, the easier it will be for me to fix:
- be able to reproduce the bug consistently
- run ISO Master from a terminal window and send me the output (if any) that is printed there
- if it only happens with one image file, tell me what it is and, if it's freely available, where i can get it
- it's better to give me too much detail then too little

Credits
-------
See CREDITS.TXT or Help/About/Credits for a list of contributors to ISO Master.

License
-------
The source code for this version of ISO Master and bkisofs is distributed under the GNU General Public Licence version 2. See LICENCE.TXT for full text.
