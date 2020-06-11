#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "../headers/fat_variables.h"
#include "../headers/fat_prototypes.h"

int compare_names(uint8_t *name1, uint8_t *ext1, char *name2, char *ext2) {
	int i;
	
	for (i = 0; i < 8; i++)
		if (toupper(name1[i]) != toupper(name2[i]))
			break;
	
	if (i < 8) return 0;
		
	for (i = 0; i < 3; i++)
		if (toupper(ext1[i]) != toupper(ext2[i]))
			break;
			
	return i == 3;
}
