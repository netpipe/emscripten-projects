#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

uint16_t calcclusters() {
	// Calc total number of clusters (to check FAT type)
	// Algorithm -- https://goo.gl/le5M2G (wiki "FAT")
	
	return ((
		(mbrSector.total_sectors_short ? mbrSector.total_sectors_short : mbrSector.total_sectors_long) -
		mbrSector.reserved_sectors - mbrSector.number_of_fats * mbrSector.fat_size_sectors -
		(mbrSector.root_dir_entries * sizeof(struct TfileRecord) / mbrSector.sector_size)) /
		mbrSector.sectors_per_cluster
		);
}

uint16_t findfreecluster(uint16_t from, FILE *img, uint32_t fat_start, int fattype) {
	// Find free clusters in image
	
	uint16_t totalclusters;
	uint16_t i;
	
	totalclusters = calcclusters();
	for (i = from; i < totalclusters; i++)
		if (readfromfat(img, 0, i, fat_start, fattype) == 0)
			return i;
	
	return (1 << fattype) - 1; // if free clusters not found -> return 0xFFF or 0xFFFF
}
