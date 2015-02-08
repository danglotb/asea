#include "bloc_allocator.h"

#define _SUPER_BLOC_MAGIC_NUMBER 0x59B5
#define _FREE_BLOC_MAGIC_NUMBER 0xD6B1

#define ERROR_SUPER_BAD_STATE "Super bloc has bad state"
#define ERROR_ALREADY_FREE "Bloc already free"

unsigned int current_vol;
struct super_bloc_t current_super_bloc;

void init_super(unsigned int vol) {
	unsigned char buffer[SECTOR_SIZE];

	struct super_bloc_t* sb = (struct super_bloc_t*)buffer;
	struct free_bloc_t* fb = (struct free_bloc_t*)buffer;

	sb->_magic_number = _SUPER_BLOC_MAGIC_NUMBER;

	sb->sb_serial_number = 0;

	snprintf(sb->sb_name, VOLUME_NAME_LENGTH_MAX, "Volume %d", vol);

	sb->sb_first_free_bloc = SUPER_BLOC_NUMBER+1;

	sb->sb_nb_free_bloc = mbr.mbr_vol[vol].vol_nb_sector-1;

	write_bloc(vol, SUPER_BLOC_NUMBER, buffer);

	fb->_magic_number = _FREE_BLOC_MAGIC_NUMBER;

	fb->fb_next_free = 0;

	fb->fb_nb_bloc = mbr.mbr_vol[vol].vol_nb_sector-1;

	write_bloc(vol, SUPER_BLOC_NUMBER+1, buffer);
}

int load_super(unsigned int vol) {
	unsigned char buffer[SECTOR_SIZE];
	struct super_bloc_t* sb = (struct super_bloc_t*)buffer;
	read_bloc(vol, SUPER_BLOC_NUMBER, buffer);
	if(sb->_magic_number != _SUPER_BLOC_MAGIC_NUMBER) {
		return RETURN_FAILURE;
	}

	memcpy(&current_super_bloc, buffer, sizeof(struct super_bloc_t));

	current_vol = vol;

	return RETURN_SUCCESS;
}

void save_super() {
	unsigned char buffer[SECTOR_SIZE];

	PRINT_ASSERT_ERROR_MSG(current_super_bloc._magic_number == _SUPER_BLOC_MAGIC_NUMBER, "Can't write super bloc into memory : " ERROR_SUPER_BAD_STATE);

	memcpy(buffer, &current_super_bloc, sizeof(struct super_bloc_t));

	write_bloc(current_vol, SUPER_BLOC_NUMBER, buffer);
}

unsigned int new_bloc() {
	unsigned char buffer[SECTOR_SIZE];
	unsigned int ret;

	struct free_bloc_t* fb = (struct free_bloc_t*)buffer;

	PRINT_ASSERT_ERROR_MSG(current_super_bloc._magic_number == _SUPER_BLOC_MAGIC_NUMBER, "Can't allocate new bloc : " ERROR_SUPER_BAD_STATE);


	if(current_super_bloc.sb_first_free_bloc == 0) { /*not space remain*/
		return 0;
	}

	read_bloc(current_vol, current_super_bloc.sb_first_free_bloc, buffer);

	PRINT_ASSERT_ERROR_MSG(fb->_magic_number == _FREE_BLOC_MAGIC_NUMBER, "Bloc error : check failed");

	ret = current_super_bloc.sb_first_free_bloc;

	current_super_bloc.sb_nb_free_bloc--;

	fb->fb_nb_bloc--;

	if(fb->fb_nb_bloc == 0) {
		current_super_bloc.sb_first_free_bloc = fb->fb_next_free;
	}
	else {
		current_super_bloc.sb_first_free_bloc++;
		write_bloc(current_vol, current_super_bloc.sb_first_free_bloc, buffer);
	}

	/*erase magic number for check the double free*/
	fb->_magic_number = 0;
	write_bloc(current_vol, ret, buffer);

	save_super();

	return ret;
}

void free_bloc(unsigned int bloc) {
	unsigned char buffer[SECTOR_SIZE];

	struct free_bloc_t* fb = (struct free_bloc_t*)buffer;

	PRINT_ASSERT_ERROR_MSG(current_super_bloc._magic_number == _SUPER_BLOC_MAGIC_NUMBER, "Can't free bloc : " ERROR_SUPER_BAD_STATE);

	read_bloc(current_vol, bloc, buffer);

	PRINT_ASSERT_ERROR_MSG(fb->_magic_number != _FREE_BLOC_MAGIC_NUMBER, "Can't free bloc : " ERROR_ALREADY_FREE);

	fb->_magic_number = _FREE_BLOC_MAGIC_NUMBER;

	fb->fb_next_free = current_super_bloc.sb_first_free_bloc;
	fb->fb_nb_bloc = 1;

	write_bloc(current_vol, bloc, buffer);

	current_super_bloc.sb_first_free_bloc = bloc;

	current_super_bloc.sb_nb_free_bloc++;

	save_super();
}

void free_bloc_array(unsigned int* blocs, unsigned int number) {
	int i;
	for(i = 0; i < number; i++) {
		if(blocs[i] != BLOC_NULL) {
			free_bloc(blocs[i]);
		}
	}
}
