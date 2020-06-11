#pragma once
#ifndef FAT_VARIABLES_H
#define FAT_VARIABLES_H

struct mbrSector {
	uint8_t sys_jmp[3];
	uint64_t oem;
	uint16_t sector_size; // always 0x200 (512)
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t number_of_fats;
	uint16_t root_dir_entries;
	uint16_t total_sectors_short;
	uint8_t media_descriptor;
	uint16_t fat_size_sectors;
	uint16_t sectors_per_track;
	uint16_t number_of_heads;
	uint32_t hidden_sectors;
	uint32_t total_sectors_long;
	
	uint8_t drive_number;
	uint8_t windows_nt_reserved;
	uint8_t extended_boot_number;
	uint32_t volume_id;
	char volume_label[11];
	char fs_type[8];
	char boot_code[448];
	uint16_t signature;
}  __attribute__((packed));

struct TfileRecord {
	uint8_t filename[8];
	uint8_t ext[3];
	uint8_t attributes;
	uint8_t reserved[10];
	uint16_t modify_time;
	uint16_t modify_date;
	uint16_t starting_cluster;
	uint32_t file_size;
} __attribute__((packed));

/* struct fatSector {
	uint8_t first_byte;
	uint8_t start_chs[3];
	uint8_t partition_type;
	uint8_t end_chs[3];
	uint32_t start_sector;
	uint32_t length_sectors;
}; //__attribute__((packed)); */

extern int func_num;

#endif // FAT_VARIABLES_H
