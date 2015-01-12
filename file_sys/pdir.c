#include "dir.h"
#include "mount.h"

int main(int argc, char** argv) {
	int inumber;

	if(argc != 2) {
		printf("./%s <absolute_path>\n", argv[0]);
		return EXIT_FAILURE;
	}

	mount();

	if((inumber = inumber_of_path(argv[1])) != 0) {
		file_desc_t fd;
		if(open_ifile(&fd, inumber) != RETURN_FAILURE) {
			struct entry_s entry;

			while(read_ifile(&fd, &entry, sizeof(struct entry_s)) > 0) {
				if(entry.inumber != 0)
					printf("%s\n", entry.name);
			}
			close_ifile(&fd);
		}
	}
	else
		 PRINT_FATAL_ERROR("Not a valid path");

	return EXIT_SUCCESS;
}
