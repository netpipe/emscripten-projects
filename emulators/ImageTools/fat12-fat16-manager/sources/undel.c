#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

int fat_undel(FILE *img, uint32_t root_start, uint32_t fat_start, int nrec, int fattype) {
	// "undel" function helper
	
	uint16_t lastcluster, cluster_size;
	uint32_t needclusters;
	struct TfileRecord fr;
	int i, j;
	char fst;
	
	fseek(img, root_start + nrec * sizeof(struct TfileRecord), SEEK_SET);
	fread(&fr, sizeof(struct TfileRecord), 1, img);
	cluster_size = mbrSector.sectors_per_cluster * mbrSector.sector_size;
	needclusters = (fr.file_size + cluster_size - 1) / cluster_size; // calc how many clusters needed for the file
	
	// Check if there are enough free clusters to recover deleted file
	for (i = fr.starting_cluster; i < fr.starting_cluster + needclusters; i++)
		if (readfromfat(img, 0, i, fat_start, fattype) != 0)
			break;
	
	// If not -> file either fragmented or rewrited -> not able to recover
	if (i < fr.starting_cluster + needclusters) {
		printf("Unable undelete file, can't restore FAT chain.\n");
		return -1;
	}
	
	printf("File: %c%c%c%c%c%c%c%c.%c%c%c\n",
			fr.filename[0],
			fr.filename[1],
			fr.filename[2],
			fr.filename[3],
			fr.filename[4],
			fr.filename[5],
			fr.filename[6],
			fr.filename[7],
			fr.ext[0],
			fr.ext[1],
			fr.ext[2]
			);
	
	/* printf("Size: %d\n", fr.file_size);	
	printf("Starting cluster: %d\n", fr.starting_cluster); */
	printf("Enter first letter: ");
	
	/* __fpurge(stdin);		// clear stdin
	scanf("%c", &fst);
	__fpurge(stdin); */

	scanf(" %c", &fst);
	getchar();
	
	fst = toupper(fst);
	lastcluster = (1 << fattype) - 8;
	fseek(img, root_start + nrec * sizeof(struct TfileRecord), SEEK_SET);
	fwrite(&fst, 1, 1, img);
	
	for (j = 0; j < mbrSector.number_of_fats; j++) {
		// For all clusters besides the last one
		for (i = fr.starting_cluster; i < fr.starting_cluster + needclusters - 1; i++)
			writetofat(img, j, i, i + 1, fat_start, fattype);
		writetofat(img, j, i, lastcluster + 7, fat_start, fattype); // the last is 0xFFF or 0xFFFF
	}
	
	return 0;
}

int undel(int c_args, char **v_args) {
	// Undelete (recover files) from FAT image
	
	FILE *img;
	uint16_t nclust;
	uint32_t fat_start, root_start;
	int fattype;
	int i, j, ndel, r;
	
	if (c_args != 2) {
		printf("undel: missing operand\nSee help for additional info.\n");
		return -1;
	}
	
	if ((img = fopen(v_args[2], "rb+")) == NULL) {
		printf("File system image file <%s> not found!\n", v_args[2]);
		return -1;
	}
	
	if (func_num == 4) printf("Opening <%s>... ", v_args[2]);
	
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
	
	if (func_num == 4) printf("Done.\n");
	
	fat_start = (mbrSector.reserved_sectors) * mbrSector.sector_size;
	root_start = fat_start + mbrSector.fat_size_sectors * mbrSector.number_of_fats * mbrSector.sector_size;
	
	printf("FAT starts at %08X, ROOT at %08X.\n", fat_start, root_start);
	printf("Recovering deleted file... \n");
	printf("-----\nEnter number of the file to recover:\n");
	
	fseek(img, root_start, SEEK_SET);
	j = 0;																		// number of deleted files
	
	for (i = 0; i < mbrSector.root_dir_entries; i++) {
		fread(&TfileRecord, sizeof(struct TfileRecord), 1, img);
		if (TfileRecord.filename[0] == 0xE5) {
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
		printf("No deleted files!\n-----\n");
		return -1;
	}
	
	// Input number of deleted file
	while (1) {
		if (1 == scanf("%d", &ndel))
			if (ndel >= 1 && ndel <= j)
				break;
			else
				printf("Wrong number! Try again.\n");
		else {
			scanf("%*s");
			printf("Wrong input data type! Try again.\n");
		}
	}
	
	fseek(img, root_start, SEEK_SET);
	
	for (j = 0, i = 0; j < ndel; i++) {
		fread(&TfileRecord, sizeof(struct TfileRecord), 1, img);
		if (TfileRecord.filename[0] == 0xE5) j++;
	}
	
	r = fat_undel(img, root_start, fat_start, i - 1, fattype); // i-1 = nrec = number of the file to recover
	
	printf("-----\nDone.\n");
	
	fclose(img);
	return r;
}
