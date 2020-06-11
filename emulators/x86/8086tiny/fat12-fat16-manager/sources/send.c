#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

int fat_write(FILE *img, FILE *sFile, uint32_t fat_start, uint32_t data_start, uint32_t cluster_size, int nrec, char *fn, int fattype) {
	// "send" function helper
	
	struct TfileRecord fr;
	uint8_t *buffer;
	uint16_t totalclusters, freeclusters;
	uint16_t cluster, old_cluster, lastcluster;
	uint32_t needclusters;
	uint32_t file_left;
	int i;
	size_t bytes_to_write, fsize;
	
	buffer = (uint8_t *) malloc(cluster_size); // buffer for 1 cluster
	memset(&fr, 0, sizeof(struct TfileRecord));
	memcpy(&fr, fn, 11);
	fseek(sFile, 0, SEEK_END);
	fsize = ftell(sFile);
	fr.file_size = fsize;
	needclusters = (fsize + cluster_size - 1) / cluster_size; // number of clusters, needed for the file
	totalclusters = calcclusters();
	
	// Calc number of free clusters total
	freeclusters = 0;
	for (i = 0; i < totalclusters; i++)
		if (readfromfat(img, 0, i, fat_start, fattype) == 0)
			freeclusters++;
	
	if (needclusters > freeclusters) {
		printf("Not enough space!\n");
		return -1;
	}
	
	lastcluster = (1 << fattype) - 8; // 0xFF8 for FAT12 and 0xFFF8 for FAT16
	fr.starting_cluster = findfreecluster(0, img, fat_start, fattype); // find first cluster for the file
	fseek(img, fat_start + mbrSector.number_of_fats * mbrSector.fat_size_sectors * mbrSector.sector_size + nrec * sizeof(struct TfileRecord), SEEK_SET); // SEEK_SET at needed root entry
	fwrite(&fr, sizeof(struct TfileRecord), 1, img);
	cluster = fr.starting_cluster;
	fseek(sFile, 0, SEEK_SET);
	
	file_left = fsize;
	
	do {
		memset(buffer, 0, cluster_size);
		bytes_to_write = fread(buffer, 1, cluster_size, sFile); // trying to read whole cluster, get the actual amount of bytes read
		fseek(img, data_start + (cluster - 2) * cluster_size, SEEK_SET);
		fwrite(buffer, cluster_size, 1, img); // writing to image
		
		printf("Copying cluster %d\n", cluster);
		
		file_left = file_left - bytes_to_write;
		old_cluster = cluster;
		
		// If current cluster is not the last
		if (file_left) cluster = findfreecluster(cluster + 1, img, fat_start, fattype);
		// If current cluster is last -> cluster = 0xFFF for FAT12 and 0xFFFF for FAT16
		else cluster = lastcluster + 7;
		
		// Writing number of cluster into the FAT tables
		for (i = 0; i < mbrSector.number_of_fats; i++) writetofat(img, i, old_cluster, cluster, fat_start, fattype);
	} while (file_left);
	
	printf("Done.\n");
	free(buffer);
	return 0;
}

int send(int c_args, char **v_args) {
	// Write to FAT image from regular directory
	
	FILE *img, *sFile;
	uint16_t nclust;
	uint32_t fat_start, root_start, data_start;
	int fattype, flag = 0;
	int i, j, firstclear = -1, firstdeleted = -1, fexist = 0, r;
	char filename[9] = "        ", file_ext[4] = "   ", fn[12], fn2[13], cwd[1024];
	struct dirent *ent;
	DIR *dir;
	
	if (c_args != 3) {
		printf("send: missing operand\nSee help for additional info.\n");
		return -1;
	}
	
	if ((img = fopen(v_args[2], "rb+")) == NULL) {
		printf("File system image file <%s> not found!\n", v_args[2]);
		return -1;
	}
	
	for (i = 0; (i < 8) && (v_args[3][i] != '.') && (v_args[3][i] != '\0'); i++)
		filename[i] = v_args[3][i];
	for (j = 1; (j <= 3) && (v_args[3][i + j] != '\0'); j++)
		file_ext[j - 1] = v_args[3][i + j];
	
	if (func_num == 2) printf("Opening <%s>... ", v_args[2]);
	
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
	
	if (func_num == 2) printf("Done.\n");
	printf("Looking for <%s>... ", fn2);
	
	// Cheking if file exists in current directory
	if (getcwd(cwd, sizeof(cwd)) == NULL) perror("getcwd() error");
		
	if ((dir = opendir(cwd)) != NULL) {
		while ((ent = readdir(dir)) != NULL)
			if ((ent->d_type != DT_DIR) && (strcmp(ent->d_name, fn2) == 0)) {
				printf("Done.\nFile found!\n");
				closedir(dir);
				flag = 1;
				break;
			}
			
		if (!flag) {
			printf("Done.\nFile NOT found!\n");
			return -1;
		}
	} else {
		perror ("opendir() error");
		exit(EXIT_FAILURE);
	}
	
	fat_start = (mbrSector.reserved_sectors) * mbrSector.sector_size;
	root_start = fat_start + mbrSector.fat_size_sectors * mbrSector.number_of_fats * mbrSector.sector_size;
	data_start = root_start + mbrSector.root_dir_entries * sizeof(struct TfileRecord);
	
	printf("FAT starts at %08X, ROOT at %08X, DATA at %08X.\n", fat_start, root_start, data_start);
	
	fseek(img, root_start, SEEK_SET);
	
	// Checking if the file already exists on FAT image and looking for a free entry
	for (i = 0; i < mbrSector.root_dir_entries; i++) {
		fread(&TfileRecord, sizeof(struct TfileRecord), 1, img);
		if (compare_names(TfileRecord.filename, TfileRecord.ext, filename, file_ext))
			fexist = 1;
		if (0 == TfileRecord.filename[0] && firstclear == -1)
			firstclear = i;
		if (0xE5 == TfileRecord.filename[0] && firstdeleted == -1)
			firstdeleted = i;
	}
	
	if (fexist) {
		printf("File exists!\n");
		return -1;
	}
	
	if (firstclear == -1 && firstdeleted == -1) {
		printf("Root is full!\n");
		return -1;
	}
	
	sFile = fopen(v_args[3], "rb");
	strcpy(fn, filename);
	strcat(fn, file_ext);
	
	for (i = 0; fn[i]; i++) fn[i] = toupper(fn[i]);
		
	r = fat_write(img, sFile, fat_start, data_start, mbrSector.sectors_per_cluster * mbrSector.sector_size,
			firstclear != -1 ? firstclear : firstdeleted,
			fn,
			fattype
		);
		
	fclose(sFile);
	fclose(img);
	return r;
}
