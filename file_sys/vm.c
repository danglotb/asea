#include <stdio.h>
#include <stdlib.h>

#include "mbr.h"

#define HDA_CFG_FILE "hardware.ini"

#define SCAN(STR, VAR, DEFAULT) \
	{ \
		int _scan_err; \
		_scan_err = scanf(STR, &VAR); \
		if(_scan_err == EOF) { \
			exit(EXIT_FAILURE); \
		} \
		else if(_scan_err == 0) { \
			getchar(); \
			VAR = DEFAULT; \
		} \
	}


enum cmd_e {
	CMD_VOL_DISPLAY = 1,
	CMD_VOL_CREATE,
	CMD_VOL_DELETE,
	CMD_VOL_FORMAT,
	CMD_VOL_EXIT
};

void print_menu() {
	printf("\n");
	printf("1. Display volumes\n");
	printf("2. Create volume\n");
	printf("3. Delete volume\n");
	printf("4. Format volume\n");
	printf("5. Exit\n");
}

void cmd_display() {
	unsigned int i;

	printf("Volume list : \n");

	for(i=0; i<mbr.mbr_nb_vol; i++) {
		printf("\t[Volume %d] block number : %d \tstart : (cylinder %d, sector %d)\tend : (cylinder %d, sector %d)\n", i+1, mbr.mbr_vol[i].vol_nb_sector, mbr.mbr_vol[i].vol_first_cylinder, mbr.mbr_vol[i].vol_first_sector, BLOCK_TO_CYLINDER(i, mbr.mbr_vol[i].vol_nb_sector), BLOCK_TO_SECTOR(i, mbr.mbr_vol[i].vol_nb_sector));
	}

	if(mbr.mbr_nb_vol == 0)
		printf("\tNo volume found.\n");
}

void cmd_create() {
	unsigned int i, max_block, scan_ok;
	int block_number, first_cylinder, first_sector;

	if(mbr.mbr_nb_vol+1 >= MAX_VOLUME) {
		printf("Volume number limit reached\n");
		return;
	}

	scan_ok = 0;

	while(!scan_ok) {
		printf("> start cylinder ? [0 - %d] ", NB_CYLINDER-1);
		fflush(stdout);
		SCAN("%d", first_cylinder, -1);
		printf("\n");

		if(first_cylinder >= NB_CYLINDER || first_cylinder < 0) {
			printf("Incorrect start cylinder.\n");
		}
		else
			scan_ok = 1;

	}

	scan_ok = 0;

	while(!scan_ok) {
		printf("> start sector ? [0 - %d] ", NB_SECTOR-1);
		fflush(stdout);
		SCAN("%d", first_sector, -1);
		printf("\n");

		if(first_sector >= NB_SECTOR || first_sector < 0) {
			printf("Incorrect start sector.\n");
		}
		else
			scan_ok = 1;
	}

	scan_ok = 0;

	max_block = NB_CYLINDER*NB_SECTOR-(first_cylinder*NB_SECTOR+first_sector);

	while(!scan_ok) {
		printf("> block number ? [1 - %d] ", max_block);
		fflush(stdout);
		SCAN("%d", block_number, -1);
		printf("\n");

		if(block_number <= 0 || block_number > max_block) {
			printf("Incorrect block number.\n");
		}
		else
			scan_ok = 1;
	}

	if(first_cylinder == 0 && first_sector == 0) {
		printf("Can't start volume on the MBR sector (cylinder = 0 and sector = 0)\n");
		return;
	};

	for(i=0; i<mbr.mbr_nb_vol; i++) {
		unsigned int collision = 0;
		if(first_cylinder*NB_SECTOR+first_sector > mbr.mbr_vol[i].vol_first_cylinder*NB_SECTOR+mbr.mbr_vol[i].vol_first_sector) {
			if(first_cylinder*NB_SECTOR+first_sector < mbr.mbr_vol[i].vol_first_cylinder*NB_SECTOR+mbr.mbr_vol[i].vol_first_sector+mbr.mbr_vol[i].vol_nb_sector)
				collision = 1;
		}
		else {
			if(first_cylinder*NB_SECTOR+first_sector+block_number > mbr.mbr_vol[i].vol_first_cylinder*NB_SECTOR+mbr.mbr_vol[i].vol_first_sector)
				collision = 1;
		}

		if(collision) {
			printf("Collision detected with volume %d\n", i+1);
			return;
		}
	}

	mbr.mbr_vol[mbr.mbr_nb_vol].vol_first_cylinder = first_cylinder;
	mbr.mbr_vol[mbr.mbr_nb_vol].vol_first_sector = first_sector;
	mbr.mbr_vol[mbr.mbr_nb_vol].vol_nb_sector = block_number;

	mbr.mbr_nb_vol++;


	save_mbr();

}

void cmd_delete() {

	unsigned int i, scan_ok;
	int index_vol;

	if(mbr.mbr_nb_vol == 0) {
		printf("No volume found.\n");
		return;
	}

	cmd_display();

	scan_ok = 0;

	while(!scan_ok) {
		printf("> volume number to delete ? [1 - %d] ", mbr.mbr_nb_vol);
		fflush(stdout);
		SCAN("%d", index_vol, 0);
		printf("\n");

		if(index_vol > mbr.mbr_nb_vol|| index_vol <= 0) {
			printf("Incorrect volume number.\n");
		}
		else
			scan_ok = 1;
	}

	index_vol--;

	for(i = index_vol; i<mbr.mbr_nb_vol-1; i++) {
		memcpy(&mbr.mbr_vol[i],  &mbr.mbr_vol[i+1], sizeof(struct vol_descr_s));
	}

	mbr.mbr_nb_vol--;

	save_mbr();

}

void cmd_format() {

	unsigned int scan_ok;
	int index_vol;

	if(mbr.mbr_nb_vol == 0) {
		printf("No volume found.\n");
		return;
	}

	cmd_display();

	scan_ok = 0;

	while(!scan_ok) {
		printf("> volume number to format ? [1 - %d] ", mbr.mbr_nb_vol);
		fflush(stdout);
		SCAN("%d", index_vol, 0);
		printf("\n");

		if(index_vol > mbr.mbr_nb_vol|| index_vol <= 0) {
			printf("Incorrect volume number.\n");
		}
		else
			scan_ok = 1;
	}

	format_vol(index_vol-1);

	save_mbr();

}

int main() {

	printf("\tVolume Manager\n\n");
    sem_init(&lock_disk, 1); 

    boot();
    load_mbr();

	while(1) {
		int choice;

		print_menu();

		printf("\n");
		printf("> choice ? [%d - %d] ", CMD_VOL_DISPLAY, CMD_VOL_EXIT);
		fflush(stdout);

		SCAN("%d", choice, 0);
		printf("\n");

		switch(choice) {

		case CMD_VOL_DISPLAY:
			cmd_display();
			break;
		case CMD_VOL_CREATE:
			cmd_create();
			break;
		case CMD_VOL_DELETE:
			cmd_delete();
			break;
		case CMD_VOL_FORMAT:
			cmd_format();
			break;
		case CMD_VOL_EXIT:
			exit(EXIT_SUCCESS);
		default:
			printf("Incorrect choice \"%d\"\n", choice);
		}


	}

}
