#include <stdio.h>
#include "dir.h"

int ddir(int inumber) {
    unsigned int status;
    file_desc_t fd;

	mount();

    path_cursor = strlen(argv[1])-1;

    while(path_cursor >= 0 && argv[1][path_cursor] != '/') {
        path_cursor--;
    }


    if(path_cursor >= 0 && argv[1][path_cursor+1] != '\0') {
        int idir;
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
        
        if((idir = inumber_of_path(argv[1])) == 0) {
            PRINT_FATAL_ERROR("Not a valid path");
        }

        if(del_entry(idir, name) == -1) {
            PRINT_FATAL_ERROR("Unable to delete file");
        }
        
    }
    else
        PRINT_FATAL_ERROR("Not a valid path");


	status = open_ifile(&fd, inumber);
   	ffatal(!status, "erreur ouverture fichier %d", inumber);

    

	return EXIT_SUCCESS;
}

static void
usage(const char *prgm)
{
    fprintf(stderr, "[%s] usage:\n\t"
            "%s inumber\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
    unsigned int inumber;
    unsigned int errno;
    
    if (argc != 2)
        usage(argv[0]);

    errno = 0;
    inumber = strtol(argv[1], NULL, 10);
    if (errno || inumber <= 0)
        usage(argv[0]);

    ddir(inumber);
    
    exit(EXIT_SUCCESS);         
}
