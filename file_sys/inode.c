#include "inode.h"

void read_inode(unsigned int inumber, struct inode_s* inode) {
	unsigned char buffer[SECTOR_SIZE];

	PRINT_ASSERT_ERROR_MSG(inumber != BLOC_NULL, "Null inode (read_inode)");
	read_bloc(current_vol, inumber, buffer);
	memcpy(inode, buffer, sizeof(struct inode_s));
}

void write_inode(unsigned int inumber, const struct inode_s* inode) {
	unsigned char buffer[SECTOR_SIZE];

	PRINT_ASSERT_ERROR_MSG(inumber != BLOC_NULL, "Null inode (write_inode)");
	memcpy(buffer, inode, sizeof(struct inode_s));
	write_bloc(current_vol, inumber, buffer);
}

unsigned int create_inode(enum file_type_e type) {
	struct inode_s inode;
	unsigned int inumber;
	memset(&inode, 0, sizeof(struct inode_s));
	inode.inode_type = type;

	inumber = new_bloc();

	if(inumber == BLOC_NULL) {
		fprintf(stderr, "Impossible to create new inode. no space remain\n");
		return BLOC_NULL;
	}

	write_inode(inumber, &inode);

	return inumber;
}

int delete_inode(unsigned int inumber) {
	struct inode_s inode;
	unsigned int buffer_indirect_1[SECTOR_SIZE];
	unsigned int buffer_indirect_2[SECTOR_SIZE];
	unsigned int i;

	PRINT_ASSERT_ERROR_MSG(inumber != BLOC_NULL, "Null inode (delete_inode)");

	read_inode(inumber, &inode);

	/* Direct */
	free_bloc_array(inode.inode_direct_bloc, NB_DIRECT_BLOC);

	/* Indirect 1 */
	if(inode.inode_indirect_1 != BLOC_NULL) {
		read_bloc(current_vol, inode.inode_indirect_1, (unsigned char*)buffer_indirect_1);

		free_bloc_array(buffer_indirect_1, NB_INDIRECT_BLOC);
		free_bloc(inode.inode_indirect_1);
	}

	/* Indirect 2 */
	if(inode.inode_indirect_2 != BLOC_NULL) {
		read_bloc(current_vol, inode.inode_indirect_2, (unsigned char*)buffer_indirect_2);

		for(i=0; i<NB_INDIRECT_BLOC; i++) {
			if(buffer_indirect_2[i] != BLOC_NULL) {
				read_bloc(current_vol, buffer_indirect_2[i], (unsigned char*)buffer_indirect_1);

				free_bloc_array(buffer_indirect_1, NB_INDIRECT_BLOC);
				free_bloc(buffer_indirect_2[i]);
			}

		}
		free_bloc(inode.inode_indirect_2);
	}
	free_bloc(inumber);

	return RETURN_SUCCESS;
}

unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, bool_t do_allocate) {
	struct inode_s inode;
	unsigned int buffer_indirect[SECTOR_SIZE];
	unsigned int buffer_indirect2[SECTOR_SIZE];

	PRINT_ASSERT_ERROR_MSG(inumber != BLOC_NULL, "Null inode (vbloc_of_fbloc)");

	read_inode(inumber, &inode);

	if(fbloc < NB_DIRECT_BLOC) { /* Direct */
		if(inode.inode_direct_bloc[fbloc] == BLOC_NULL) {
			if(do_allocate) {
				inode.inode_direct_bloc[fbloc] = new_bloc();
				write_inode(inumber, &inode);
			}
			else {
				return BLOC_NULL;
			}
		}
		return inode.inode_direct_bloc[fbloc];



	}
	else if(fbloc < NB_INDIRECT_BLOC + NB_DIRECT_BLOC) { /* Indirect 1 */

		fbloc -= NB_DIRECT_BLOC;

		unsigned int vbloc;

		/* Level 0 */
		if(inode.inode_indirect_1 == BLOC_NULL) {
			if(do_allocate) {
				inode.inode_indirect_1 = new_bloc();
				if(inode.inode_indirect_1 == BLOC_NULL) {
					return BLOC_NULL;
				}
				/* set 0 to new bloc */
				memset(buffer_indirect, 0, SECTOR_SIZE);
				write_bloc(current_vol, inode.inode_indirect_1, (unsigned char*)buffer_indirect);

				write_inode(inumber, &inode);
			}
			else {
				return BLOC_NULL;
			}
		}


		/* Level 1 */
		read_bloc(current_vol, inode.inode_indirect_1, (unsigned char*)buffer_indirect);

		vbloc = buffer_indirect[fbloc];

		if(vbloc == BLOC_NULL) {
			if(do_allocate) {
				buffer_indirect[fbloc] = new_bloc();
				if(buffer_indirect[fbloc] == BLOC_NULL) {
					return BLOC_NULL;
				}
				write_bloc(current_vol, inode.inode_indirect_1, (unsigned char*)buffer_indirect);
				vbloc = buffer_indirect[fbloc];
			}
			else {
				return BLOC_NULL;
			}
		}
		return vbloc;
	}
	else if (fbloc < NB_INDIRECT_BLOC + NB_DIRECT_BLOC + NB_INDIRECT_BLOC*NB_INDIRECT_BLOC) { /* Indirect 2 */

		unsigned int vbloc_1, vbloc_2;

		fbloc -= NB_DIRECT_BLOC+NB_INDIRECT_BLOC;

		/* Level 0 */
		if(inode.inode_indirect_2 == BLOC_NULL) {
			if(do_allocate) {
				inode.inode_indirect_2 = new_bloc();
				if(inode.inode_indirect_2 == BLOC_NULL) {
					return BLOC_NULL;
				}
				/* set 0 to new bloc */
				memset(buffer_indirect, 0, SECTOR_SIZE);
				write_bloc(current_vol, inode.inode_indirect_2, (unsigned char*)buffer_indirect);

				write_inode(inumber, &inode);
			}
			else {
				return BLOC_NULL;
			}
		}

		/* Level 1 */
		read_bloc(current_vol, inode.inode_indirect_2, (unsigned char*)buffer_indirect);

		vbloc_1 = buffer_indirect[fbloc/NB_INDIRECT_BLOC];

		if(vbloc_1 == BLOC_NULL) {
			if(do_allocate) {
				buffer_indirect[fbloc/NB_INDIRECT_BLOC] = new_bloc();
				if(buffer_indirect[fbloc/NB_INDIRECT_BLOC] == BLOC_NULL) {
					return BLOC_NULL;
				};
				/* set 0 to new bloc */
				memset(buffer_indirect2, 0, SECTOR_SIZE);
				write_bloc(current_vol, buffer_indirect[fbloc/NB_INDIRECT_BLOC], (unsigned char*)buffer_indirect2);

				write_bloc(current_vol, inode.inode_indirect_2, (unsigned char*)buffer_indirect);

				vbloc_1 = buffer_indirect[fbloc/NB_INDIRECT_BLOC];
			}
			else {
				return BLOC_NULL;
			}
		}

		/* Level 2 */
		read_bloc(current_vol, vbloc_1, (unsigned char*)buffer_indirect);

		vbloc_2 = buffer_indirect[fbloc%NB_INDIRECT_BLOC];

		if(vbloc_2 == BLOC_NULL) {
			if(do_allocate) {
				buffer_indirect[fbloc%NB_INDIRECT_BLOC] = new_bloc();
				if(buffer_indirect[fbloc%NB_INDIRECT_BLOC] == BLOC_NULL) {
					return BLOC_NULL;
				}
				write_bloc(current_vol, vbloc_1, (unsigned char*)buffer_indirect);
				vbloc_2 = buffer_indirect[fbloc%NB_INDIRECT_BLOC];
			}
			else {
				return BLOC_NULL;
			}
		}
		return vbloc_2;
	}
	else
		return BLOC_NULL;
}
