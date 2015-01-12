/* ------------------------------
   $Id: if_nfile.c,v 1.1 2009/11/16 05:38:07 marquet Exp $
   ------------------------------------------------------------

   Create a new file, read from stdin, return the inumber 
   Philippe Marquet, Nov 2009
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dir.h"
#include "mount.h"

static void usage(const char *prgm) {
    fprintf(stderr, "[%s] usage:\n\t"
            "%s <absolute path>\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int main (int argc, char *argv[]) {
	int path_cursor;
	char* name;

	if (argc != 2)
		usage(argv[0]);

	mount();
	path_cursor = strlen(argv[1])-1;

	while(path_cursor >= 0 && argv[1][path_cursor] != '/') {
		path_cursor--;
	}


	if(path_cursor >= 0 && argv[1][path_cursor+1] != '\0') {
		int idir, inumber, c;
		char min_name[2];
		file_desc_t fd;

		name = argv[1]+path_cursor+1;

		if(path_cursor == 0) {
			min_name[0] = '/';
			min_name[1] = '\0';
			argv[1] = min_name;
		}
		else {
			argv[1][path_cursor] = '\0';
		}
		
		if((idir = inumber_of_path(argv[1])) == 0) {
			PRINT_FATAL_ERROR("Not a valid path");
		}

		if((inumber = add_entry(idir, name, FILE_FILE)) == -1) {
			PRINT_FATAL_ERROR("Unable to create directory");
		}

		open_ifile(&fd, inumber);

		while((c=getchar()) != EOF) {
			if(writec_ifile(&fd, c) == RETURN_FAILURE) {
				break;
			}
		}

		close_ifile(&fd);
		
	}
	else
		PRINT_FATAL_ERROR("Not a valid path");

    	umount();
    
    	exit(EXIT_SUCCESS);         
}
