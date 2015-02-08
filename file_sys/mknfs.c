#include <stdio.h>
#include "dir.h"
#include "mount.h"
#include "mbr.h"
#include "bloc_allocator.h"
#include "ifile.h"

extern void load_current_volume (int fatal_e);
extern char *get_hw_config ();

int main() {
	
	 	sem_init(&lock_disk, 1); 

	    boot();
	    mount();

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


	return EXIT_SUCCESS;
}
