#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

void fat_read(FILE *img, FILE *dFile, uint32_t fat_start, uint32_t data_start, uint32_t cluster_size, int fattype) {
	// "recv" function helper
	
	uint8_t *buffer;
	uint16_t cluster, lastcluster;
	uint32_t file_left = TfileRecord.file_size;
	size_t bytes_to_write;
	
	buffer = (uint8_t *) malloc(cluster_size);
	lastcluster = (1 << fattype) - 8; // 0xFF8 for FAT12 and 0xFFF8 for FAT16
	cluster = TfileRecord.starting_cluster;
	
	do {
		fseek(img, data_start + (cluster - 2) * cluster_size, SEEK_SET);
		fread(buffer, cluster_size, 1, img);
		
		printf("Copying cluster %d\n", cluster);
		
		bytes_to_write = file_left >= cluster_size ? cluster_size : file_left;
		fwrite(buffer, 1, bytes_to_write, dFile);
		cluster = readfromfat(img, 0, cluster, fat_start, fattype);
		file_left = file_left - bytes_to_write;
	} while (file_left && cluster < lastcluster);
	
	printf("Done.\n");
	free(buffer);
}

int recv(int c_args, char **v_args) {
	// Read from FAT image and write to regular directory
	
	FILE *img, *dFile;
	uint16_t nclust;
	uint32_t fat_start, root_start, data_start;
	int fattype;
	int i, j;
	char filename[9] = "        ", file_ext[4] = "   ", fn2[13];
	
	if (c_args != 3) {
		printf("undel: missing operand\nSee help for additional info.\n");
		return -1;
	}
	
	if ((img = fopen(v_args[2], "rb")) == NULL) {
		printf("File system image file <%s> not found!\n", v_args[2]);
		return -1;
	}
	
	for (i = 0; (i < 8) && (v_args[3][i] != '.') && (v_args[3][i] != '\0'); i++)
		filename[i] = v_args[3][i];
	for (j = 1; (j <= 3) && (v_args[3][i + j] != '\0'); j++)
		file_ext[j - 1] = v_args[3][i + j];
		
	if (func_num == 1) printf("Opening <%s>... ", v_args[2]);

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
	
	i = 0;
	while ((filename[i] != ' ') && (i < 8)) {
		fn2[i] = filename[i];
		i++;
	}
	fn2[i++] = '.';
	j = 0;
	while ((file_ext[j] != ' ') && (j < 3)) {
		fn2[i] = file_ext[j];
		i++;
		j++;
	}
	fn2[i] = '\0';
	
	if (func_num == 1) printf("Done.\n");
	printf("Looking for <%s>... ", fn2);
	
	fat_start = (mbrSector.reserved_sectors) * mbrSector.sector_size;
	root_start = fat_start + mbrSector.fat_size_sectors * mbrSector.number_of_fats * mbrSector.sector_size;
	data_start = root_start + mbrSector.root_dir_entries * sizeof(struct TfileRecord);
	
	fseek(img, root_start, SEEK_SET);
	
	for (i = 0; i < mbrSector.root_dir_entries; i++) {
		fread(&TfileRecord, sizeof(struct TfileRecord), 1, img);
		if (compare_names(TfileRecord.filename, TfileRecord.ext, filename, file_ext)) {
			printf("Done.\nFile found!\n");
			break;
		}
	}

	if (i == mbrSector.root_dir_entries) {
		printf("Done.\nFile not found!\n");
		return -1;
	}
	
	printf("FAT starts at %08X, ROOT at %08X, DATA at %08X.\n", fat_start, root_start, data_start);

	dFile = fopen(v_args[3], "wb");

	fat_read(img, dFile, fat_start, data_start, mbrSector.sectors_per_cluster * mbrSector.sector_size, fattype);
	
	fclose(dFile);
	fclose(img);
	
	return 0;
}
