#pragma once
#ifndef FAT_PROTOTYPES_H
#define FAT_PROTOTYPES_H

int compare_names(uint8_t *name1, uint8_t *ext1, char *name2, char *ext2);

uint16_t calcclusters();
uint16_t findfreecluster(uint16_t from, FILE *img, uint32_t fat_start, int fattype);

uint16_t readfromfat(FILE *img, int nfat, uint16_t cluster, uint32_t fat_start, int fattype);
void writetofat(FILE *img, int nfat, uint16_t cluster, uint16_t val, uint32_t fat_start, int fattype);

void fat_read(FILE *img, FILE *dFile, uint32_t fat_start, uint32_t data_start, uint32_t cluster_size, int fattype);
int fat_write(FILE *img, FILE *sFile, uint32_t fat_start, uint32_t data_start, uint32_t cluster_size, int nrec, char *fn, int fattype);
void fat_del(FILE *img, uint32_t root_start, uint32_t fat_start, int nrec, int fattype);
int fat_undel(FILE *img, uint32_t root_start, uint32_t fat_start, int nrec, int fattype);

int recv(int c_args, char **v_args);
int send(int c_args, char **v_args);
int del(int c_args, char **v_args);
int undel(int c_args, char **v_args);

int showdir(int c_args, char *line);
int parse(char *line, char **v_args);

void help();

#endif // FAT_PROTOTYPES_H
