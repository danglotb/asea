#include "mbr.h"

#define _MBR_MAGIC_NUMBER 0x9A56

void load_mbr() {
	unsigned char buffer[SECTOR_SIZE];

	PRINT_ASSERT_ERROR_MSG(SECTOR_SIZE > sizeof(mbr_t), "MBR Structure size (struct size need to be lower than sector size)");

	read_sector(0, 0, buffer);

	memcpy(&mbr, buffer, sizeof(mbr_t));

	if(mbr._magic_number != _MBR_MAGIC_NUMBER) {
		int answer_ok = 0;
		printf("Unreconized volume on HDA. Do you want initialize this HDA ? (All data will be lost) [y/n] : ");
		fflush(stdout);

		while(!answer_ok) {
			char c;
			scanf("%c", &c);
			switch(toupper(c)) {

			case 'Y':
				printf("\n");
				mbr._magic_number  = _MBR_MAGIC_NUMBER;
				mbr.mbr_nb_vol = 0;
				save_mbr();
				answer_ok = 1;
				break;
			case 'N':
				PRINT_FATAL_ERROR("\nHDA Unreconized. Shutdown ...");
			default:
				printf("\nUnreconized option \"%c\". enable choice [y/n] : ", c);
				fflush(stdout);
			}
		}

	}
}

void save_mbr() {
	unsigned char buffer[SECTOR_SIZE];

	memcpy(buffer, &mbr, sizeof(mbr_t));

	write_sector(0, 0, buffer);
}

void read_bloc(unsigned int volume, unsigned int block, unsigned char* buffer) {
	read_sector(BLOCK_TO_CYLINDER(volume, block), BLOCK_TO_SECTOR(volume, block), buffer);
}

void write_bloc(unsigned int volume, unsigned int block, const unsigned char* buffer) {
	write_sector(BLOCK_TO_CYLINDER(volume, block), BLOCK_TO_SECTOR(volume, block), buffer);
}

void format_vol(unsigned int volume) {
	PRINT_ASSERT_ERROR_MSG(volume < MAX_VOLUME, "Incorrect parameter volume (value to high).");

	format_sector(mbr.mbr_vol[volume].vol_first_cylinder, mbr.mbr_vol[volume].vol_first_sector, mbr.mbr_vol[volume].vol_nb_sector, 0);
}
