/* ------------------------------
   $Id: if_pfile.c,v 1.1 2009/11/16 05:38:07 marquet Exp $
   ------------------------------------------------------------

   Print a file identified by its inumber
   Philippe Marquet, Nov 2009
   
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dir.h"
#include "mount.h"

static void
pfile(unsigned int inumber)
{
    file_desc_t fd;
    int status;
    int c;
    
    status = open_ifile(&fd, inumber);
    ffatal(!status, "erreur ouverture fichier %d", inumber);

    while((c=readc_ifile(&fd)) != READ_EOF)
        putchar(c);

    close_ifile(&fd);
}
static void usage(const char *prgm) {
    fprintf(stderr, "[%s] usage:\n\t"
            "%s <absolute path>\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int main (int argc, char *argv[]) {
    unsigned inumber;
    
    if (argc != 2)
        usage(argv[0]);


	mount();

	if((inumber = inumber_of_path(argv[1])) == 0) {
		PRINT_FATAL_ERROR("Not a valid path");
	}

	pfile(inumber);

    	umount();
    
   	exit(EXIT_SUCCESS);         
}
