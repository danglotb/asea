/* ------------------------------
   $Id: if_cfile.c,v 1.2 2009/11/30 09:08:02 marquet Exp $
   ------------------------------------------------------------

   Copy a file given by its inumber to another file.
   Return the new file inumber.
   Philippe Marquet, Nov 2009
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dir.h"
#include "mount.h"

static void usage(const char *prgm) {
    fprintf(stderr, "[%s] usage:\n\t"
            "%s <original_file_path> <copy_file_path>\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int main (int argc, char *argv[]) {
	unsigned inumber_origin, inumber_copy;
	int path_cursor;
	char* name;
    
	if (argc != 3)
		usage(argv[0]);


	mount();

	if((inumber_origin = inumber_of_path(argv[1])) == 0) {
		PRINT_FATAL_ERROR("Not a valid path");
	}

	path_cursor = strlen(argv[2])-1;

	while(path_cursor >= 0 && argv[2][path_cursor] != '/') {
		path_cursor--;
	}


	if(path_cursor >= 0 && argv[2][path_cursor+1] != '\0') {
		int idir, c;
		char min_name[2];
		file_desc_t fd_origin, fd_copy;

		name = argv[2]+path_cursor+1;

		if(path_cursor == 0) {
			min_name[0] = '/';
			min_name[1] = '\0';
			argv[2] = min_name;
		}
		else {
			argv[2][path_cursor] = '\0';
		}
		
		if((idir = inumber_of_path(argv[2])) == 0) {
			PRINT_FATAL_ERROR("Not a valid path");
		}

		if((inumber_copy = add_entry(idir, name, FILE_FILE)) == -1) {
			PRINT_FATAL_ERROR("Unable to create directory");
		}

		open_ifile(&fd_origin, inumber_origin);

		open_ifile(&fd_copy, inumber_copy);

		while((c=readc_ifile(&fd_origin)) != READ_EOF) {
			if(writec_ifile(&fd_copy, c) == RETURN_FAILURE) {
				break;
			}
		}

		close_ifile(&fd_origin);
		close_ifile(&fd_copy);
	}
	else
		PRINT_FATAL_ERROR("Not a valid path");

    	umount();
    
   	exit(EXIT_SUCCESS);         
}
