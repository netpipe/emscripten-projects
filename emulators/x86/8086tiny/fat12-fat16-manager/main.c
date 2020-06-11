#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "./headers/fat_variables.h"
#include "./headers/fat_prototypes.h"

struct mbrSector mbrSector;
struct TfileRecord TfileRecord;

int func_num = 0;

int parse(char *line, char **v_args) {
	// Parse string
	
	int num = -1;
	int i = 1;
	
	v_args++;
	while (*line != '\0') {
		while (*line == ' ' || *line == '\t' || *line == '\n') *line++ = '\0';
			
		if (i == 2) {
			v_args++;
			*v_args++ = line;
		}
		else *v_args++ = line;
		
		i++;
		while (*line != '\0' && *line != '\t' && *line != '\n' && *line != ' ') line++;
		
		num++;
	}
	
	*v_args = '\0';
	return ++num;
}

int main(int argc, char **argv) {
	FILE *img;
	uint16_t nclust;
	size_t n = 0;
	int fattype;
	int c_args;
	char *cmd;
	char *v_args[4];
	
	if (argc < 2) {
		printf("Invalid arguments! See help for additional info.\n");
		return -1;
	}
	
	if ((strcmp(argv[0], "./fat") == 0) && (strcmp(argv[1], "help") != 0) && (argc == 2)) {
		printf("-----\nType \"help\" to show program help.\n-----\n\n");
		
		if ((img = fopen(argv[1], "rb")) == NULL) {
			printf("File system image file <%s> not found!\n", v_args[2]);
			return -1;
		}
		
		printf("Opening <%s>... ", argv[1]);
		
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
		
		printf("Done.\n");
		
		memset(&mbrSector, 0, sizeof(struct mbrSector));
		fclose(img);
		
		v_args[2] = argv[1];
		
		while (1) {
			printf("\n>_ ");
			
			if ((getline(&cmd, &n, stdin) > 1) && (!isspace(*cmd))) {
				c_args = parse(cmd, v_args);
				
				if (strcmp(v_args[1], "exit") == 0) {
					printf("Exiting program...\n");
					exit(0);
				} else if (strcmp(v_args[1], "help") == 0)
					help();
				else if (strcmp(v_args[1], "recv") == 0)
					recv(c_args, v_args);
				else if (strcmp(v_args[1], "send") == 0)
					send(c_args, v_args);
				else if (strcmp(v_args[1], "del") == 0)
					del(c_args, v_args);
				else if (strcmp(v_args[1], "undel") == 0)
					undel(c_args, v_args);
				else if (strcmp(v_args[1], "showdir") == 0)
					showdir(c_args, v_args[2]);
				else
					printf("Wrong command! See help for additional info.\n");
			}
		}
	}
	else if ((strcmp(argv[0], "./recv") == 0) && (argc == 3)) {
		func_num = 1;
		c_args = 3;
		v_args[2] = argv[1];
		v_args[3] = argv[2];
		recv(c_args, v_args);
	}
	else if ((strcmp(argv[0], "./send") == 0) && (argc == 3)) {
		func_num = 2;
		c_args = 3;
		v_args[2] = argv[1];
		v_args[3] = argv[2];
		send(c_args, v_args);
	}
	else if ((strcmp(argv[0], "./del") == 0) && (argc == 3)) {
		func_num = 3;
		c_args = 3;
		v_args[2] = argv[1];
		v_args[3] = argv[2];
		del(c_args, v_args);
	}
	else if ((strcmp(argv[0], "./undel") == 0) && (argc == 2)) {
		func_num = 4;
		c_args = 2;
		v_args[2] = argv[1];
		undel(c_args, v_args);
	}
	else if ((strcmp(argv[0], "./showdir") == 0) && (argc == 2)) {
		func_num = 5;
		c_args = 2;
		v_args[2] = argv[1];
		showdir(c_args, v_args[2]);
	}
	else if ((strcmp(argv[0], "./fat") == 0) && (strcmp(argv[1], "help") == 0) && (argc == 2))
		help();
	else printf("Invalid arguments! See help for additional info.\n");
	
	return 0;
}
