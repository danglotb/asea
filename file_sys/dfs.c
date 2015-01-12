#include <stdio.h>
#include "bloc_allocator.h"
#include "mount.h"

int main() {
	unsigned int i;

	mount();

	printf("Volume list : \n");

	for(i=0; i<mbr.mbr_nb_vol; i++) {
		printf("\t[Volume %d] block number : %d\tstart : (cylinder %d, sector %d)\tend : (cylinder %d, sector %d)\n", i, mbr.mbr_vol[i].vol_nb_sector, mbr.mbr_vol[i].vol_first_cylinder, mbr.mbr_vol[i].vol_first_sector, BLOCK_TO_CYLINDER(i, mbr.mbr_vol[i].vol_nb_sector), BLOCK_TO_SECTOR(i, mbr.mbr_vol[i].vol_nb_sector));
		if(i == current_vol) {
			printf("\t<CURRENT> name : %s\tserial number : %d\tfree block : %d (%d %%)\n", current_super_bloc.sb_name, current_super_bloc.sb_serial_number, current_super_bloc.sb_nb_free_bloc, 100*current_super_bloc.sb_nb_free_bloc/mbr.mbr_vol[i].vol_nb_sector);
		}
	}

	printf("Current Volume : %d\n", current_vol);


	return EXIT_SUCCESS;
}
