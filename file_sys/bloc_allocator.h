#ifndef _BLOC_ALLOCATOR_H
#define _BLOC_ALLOCATOR_H

#include "mbr.h"
#include "drive.h"

#define SUPER_BLOC_NUMBER 0
#define VOLUME_NAME_LENGTH_MAX 32
#define BLOC_NULL 0

struct super_bloc_t {
	unsigned int _magic_number;
	unsigned int sb_serial_number;
	char sb_name[VOLUME_NAME_LENGTH_MAX];
	unsigned int sb_first_free_bloc;
	unsigned int sb_nb_free_bloc;
	unsigned int sb_inode_root;
};

struct free_bloc_t {
	unsigned int _magic_number;
	unsigned int fb_next_free;
	unsigned int fb_nb_bloc;
};

extern unsigned int current_vol;
extern struct super_bloc_t current_super_bloc;


/*
 * Initialize a volume
 * Create the super bloc and make all blocs free
 */
void init_super(unsigned int vol);

/*
 * Load volume information into memory
 * return -1 if volume can't be read
 */
int load_super(unsigned int vol);

/*
 * Save volume information into HDA
 */
void save_super();

/*
 * Allocate a bloc in current volume
 * return 0 if no free bloc was found
 */
unsigned int new_bloc();

/*
 * Free a bloc in the current volume
 */
void free_bloc(unsigned int bloc);

/*
 * Free an array of blocs in the current volume
 */
void free_bloc_array(unsigned int* blocs, unsigned int number);

#endif
