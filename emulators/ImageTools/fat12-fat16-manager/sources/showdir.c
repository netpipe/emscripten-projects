#include <stdio.h>
#include <stdint.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

int showdir(int c_args, char *line) {
	// Show directory on FAT image
	
	FILE *img;
	uint16_t nclust;
	uint32_t fat_start, root_start;
	int fattype;
	int i, j = 0;
	
	if (c_args != 2) {
		printf("showdir: missing operand\nSee help for additional info.\n");
		return -1;
	}
	
	if ((img = fopen(line, "rb+")) == NULL) {
		printf("File system image file <%s> not found!\n", line);
		return -1;
	}
	
	if (func_num == 5) printf("Opening <%s>... ", line);
		
	fseek(img, 0x000, SEEK_SET);
	fread(&mbrSector, sizeof(struct mbrSector), 1, img);
	
	nclust = calcclusters();
	if (nclust < 4085) fattype = 12; // FAT12 type
	else if (nclust >= 4085 && nclust < 65524) fattype = 16; // FAT16 type
	else fattype = 0; // not FAT type
	if (fattype == 0) {
		printf("Unsupported file system! See help for additional info.\n");
		return -1;
	}
	
	if (func_num == 5) printf("Done.\n");
		
	fat_start = (mbrSector.reserved_sectors) * mbrSector.sector_size;
	root_start = fat_start + mbrSector.fat_size_sectors * mbrSector.number_of_fats * mbrSector.sector_size;
	
	fseek(img, root_start, SEEK_SET);
	
	printf("-----\nFiles on the disk (non-deleted):\n");
	
	for (i = 0; i < mbrSector.root_dir_entries; i++) {
		fread(&TfileRecord, sizeof(struct TfileRecord), 1, img);
		if ((TfileRecord.filename[0] != 0xE5) && (TfileRecord.filename[0] != 0) && (TfileRecord.ext[0] > 'A' && TfileRecord.ext[0] < 'Z')) {
			printf("[%d] - %c%c%c%c%c%c%c%c.%c%c%c\n",
					j + 1,
					TfileRecord.filename[0],
					TfileRecord.filename[1],
					TfileRecord.filename[2],
					TfileRecord.filename[3],
					TfileRecord.filename[4],
					TfileRecord.filename[5],
					TfileRecord.filename[6],
					TfileRecord.filename[7],
					TfileRecord.ext[0],
					TfileRecord.ext[1],
					TfileRecord.ext[2]
					);
			j++;
		}
	}
	
	if (j == 0) {
		printf("No files on image!\n-----\n");
		return -1;
	}
	
	printf("-----\n");
	return 0;
}
