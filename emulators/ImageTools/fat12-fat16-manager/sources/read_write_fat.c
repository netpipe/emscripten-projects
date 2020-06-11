#include <stdio.h>
#include <stdint.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

uint16_t readfromfat(FILE *img, int nfat, uint16_t cluster, uint32_t fat_start, int fattype) {
	// Read next cluster from FAT table
	// Algorithm -- http://www.frolov-lib.ru/books/bsp/v19/ch2_3.html
	
	uint16_t r;
	
	fseek(img, fat_start + nfat * mbrSector.fat_size_sectors * mbrSector.sector_size + cluster * (fattype / 4) / 2, SEEK_SET);
	fread(&r, 2, 1, img);
	
	if (fattype == 12) {
		// If previous cluster is NOT even -> get most significant 12 bits from next cluster
		if (cluster & 1) r = r >> 4;
		// If previous cluster is even -> get least significant 12 bits from next cluster
		else r = r & 0xfff;
	}
	
	return r;
}

void writetofat(FILE *img, int nfat, uint16_t cluster, uint16_t val, uint32_t fat_start, int fattype) {
	// Write to next cluster to FAT table
	
	uint16_t writecluster;
	
	fseek(img, fat_start + nfat * mbrSector.fat_size_sectors * mbrSector.sector_size + cluster * (fattype / 4) / 2, SEEK_SET);
	fread(&writecluster, 2, 1, img);
	
	if (fattype == 12)
		// If previous cluster is NOT even -> get least significant 4 bits from next cluster, and "OR" new value (left-shifted with 4 bits)
		if (cluster & 1) writecluster = (writecluster & 0xf) | (val << 4);
		// If previous cluster is even -> get most significant 4 bits from next cluster, and "OR" new value
		else writecluster = (writecluster & 0xf000) | val;
	// If FAT16 -> change the whole cluster
	else writecluster = val;
	
	fseek(img, -2, SEEK_CUR);
	fwrite(&writecluster, 2, 1, img);
}
