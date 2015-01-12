#include "dir.h"
#include "mount.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utility.h"
#include "inode.h"

int main(int argc, char** argv) {

	int path_cursor;
	char* name;

	if(argc != 2) {
		printf("./%s <absolute_path> <directory_name>\n", argv[0]);
		return EXIT_FAILURE;
	}

	mount();

	path_cursor = strlen(argv[1])-1;

	while(path_cursor >= 0 && argv[1][path_cursor] != '/') {
		path_cursor--;
	}


	if(path_cursor >= 0 && argv[1][path_cursor+1] != '\0') {
		unsigned int inumber;
		char min_name[2];

		name = argv[1]+path_cursor+1;

		if(path_cursor == 0) {
			min_name[0] = '/';
			min_name[1] = '\0';
			argv[1] = min_name;
		}
		else {
			argv[1][path_cursor] = '\0';
		}
		
		if((inumber = inumber_of_path(argv[1])) == 0) {
			PRINT_FATAL_ERROR("Not a valid path");
		}

		if(add_entry(inumber, name, FILE_DIRECTORY) == -1) {
			PRINT_FATAL_ERROR("Unable to create directory");
		}
		
	}
	else
		PRINT_FATAL_ERROR("Not a valid path");

	umount();

		
}
