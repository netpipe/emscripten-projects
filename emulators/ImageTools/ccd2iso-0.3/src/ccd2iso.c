/***************************************************************************
 *   Copyright (C) 2003 by Danny Kurniawan                                 *
 *   danny_kurniawan@users.sourceforge.net                                 *
 *                                                                         *
 *   Contributors:                                                         *
 *   - Kerry Harris <tomatoe-source@users.sourceforge.net>                 *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "clonecd.h"


void show_help()
{
	printf("\nUsage:\n");
	printf("        ccd2iso <.img filename> <.iso filename>\n");
	printf("        ccd2iso <option>\n\n");
	printf("option:\n");
	printf("        -?  -h  --help      show this help\n");
	printf("            -v  --version   show version number\n\n");
}


void show_version()
{
	printf("%s\n", VERSION);
}


int main(int argc, char *argv[])
{
	ccd_sector src_sect;
	int bytes_read, bytes_written, sect_num = 0;

	FILE *src_file;
	FILE *dst_file;

	if (argc == 2)
	{
		if ((strcmp(argv[1], "-?") == 0) || (strcmp(argv[1], "-h") == 0) || (strcmp(argv[1], "--help") == 0))
			show_help();
		else if ((strcmp(argv[1], "-v") == 0) || (strcmp(argv[1], "--version") == 0))
			show_version();
		else
			printf("Unknown option: %s !\n", argv[1]);
		return 0;
	}

	if ((!argv[1]) || (!argv[2]))
	{
		show_help();
		return 0;
	}

	if ((src_file = fopen(argv[1], "r")) == NULL)
	{
		printf("Error: cannot open source file for reading!\n");
		return 1;
	}
	if ((dst_file = fopen(argv[2], "w")) == NULL)
	{
		printf("Error: cannot open destination file for writing!\n");
		return 1;
	}

	while(!feof(src_file))
	{
		bytes_read = fread(&src_sect, 1, sizeof(ccd_sector), src_file);

		if (bytes_read != 0)
		{
			if (bytes_read < sizeof(ccd_sector))
			{
				printf("Error at sector %d.\n", sect_num);
				printf("The sector does not contain complete data. Sector size must be %d, while actual data read is %d\n",
					sizeof(ccd_sector), bytes_read);
				return 1;
			}

			switch (src_sect.sectheader.header.mode)
			{
				case 1:  // Mode 1 Data Sector
					bytes_written = fwrite(&(src_sect.content.mode1.data), 1, DATA_SIZE, dst_file);
					break;
				case 2:  // Mode 2 Data Sector
					bytes_written = fwrite(&(src_sect.content.mode2.data), 1, DATA_SIZE, dst_file);
					break;
				case 0xe2:
					printf("\nFound session marker, the image might contain multisession data.\n only the first session dumped.\n");
					return -1;
				default:
					printf("\nUnrecognized sector mode (%x) at sector %d!\n", src_sect.sectheader.header.mode, sect_num);
					return 1;
			}
			
			if (bytes_written < DATA_SIZE)
			{
				printf("Error writing to file.\n");
				return 1;
			}

			sect_num++;
			printf("%d sector written\r", sect_num);
		}
	}
	fclose(src_file);
	fclose(dst_file);

	printf("\nDone.\n");

	return 0;
}
