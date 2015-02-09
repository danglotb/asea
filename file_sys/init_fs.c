#include <stdio.h>
#include <stdlib.h>
#include "dir.h"
#include "mount.h"
#include "mbr.h"
#include "bloc_allocator.h"
#include "ifile.h"

extern void load_current_volume (int fatal_e);


int main(){

	printf("Main partition creation...\n");
	sem_init(&lock_disk, 1); 
	boot();
	load_mbr();
	mbr.mbr_nb_vol = 1;
	mbr.mbr_vol[0].vol_first_cylinder = 0;
	mbr.mbr_vol[0].vol_first_sector = 1;
	mbr.mbr_vol[0].vol_nb_sector = 32767;
	save_mbr();
	printf("Partition created !\n");
	printf("Initialization of file system...\n");
	load_current_volume(0);

	init_super(current_vol);

	current_super_bloc.sb_inode_root = create_ifile(FILE_DIRECTORY);

	{
		file_desc_t fd_child;
		struct entry_s entry_child;
		if(open_ifile(&fd_child, current_super_bloc.sb_inode_root) != RETURN_FAILURE) {
			entry_child.inumber = current_super_bloc.sb_inode_root;
			strcpy(entry_child.name, ".");
			write_ifile(&fd_child, &entry_child, sizeof(struct entry_s));
			strcpy(entry_child.name, "..");
			write_ifile(&fd_child, &entry_child, sizeof(struct entry_s));

			close_ifile(&fd_child);
		}
	}

	save_super();

	printf("Volume %d initialized\n", current_vol);
}