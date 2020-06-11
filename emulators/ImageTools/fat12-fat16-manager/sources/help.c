#include <stdio.h>
#include <stdint.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

void help() {
	// Help
	
	printf("Help (Part I) - From inner program console:\nLaunch program - ./a.out <fs_image.img>\n");
	printf("-- Copy file from regular directory to fat image - recv <FILE.EXT>\n");
	printf("-- Copy file from fat image to regular directory - send <FILE.EXT>\n");
	printf("-- Delete file from fat image - del <FILE.EXT>\n");
	printf("-- Undelete file on fat image - undel\n");
	printf("-- Show non-deleted files in root directory on fat image - showdir\n");
	printf("-- Exit program - exit\n");
	
	printf("\n");
	
	printf("Help (Part II) - From outer linux terminal:\nRename the object file to one of the following recv / send / del / undel / showdir.\n");
	printf("-- Copy file from regular directory to fat image - ./recv <fs_image.img> <FILE.EXT>\n");
	printf("-- Copy file from fat image to regular directory - ./send <fs_image.img> <FILE.EXT>\n");
	printf("-- Delete file fat from image - ./del <fs_image.img> <FILE.EXT>\n");
	printf("-- Undelete file on fat image - ./undel <fs_image.img>\n");
	printf("-- Show non-deleted files in root directory on fat image - ./showdir <fs_image.img>");
	
	printf("\n");
	
	printf("Supported file systems - FAT12, FAT16\n");
}
