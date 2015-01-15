#include <stdio.h>
#include "dir.h"

int ddir(char *name_dir) {
    unsigned int status, inumber;
    file_desc_t fd;
    struct entry_s entries;
    struct inode_s inode;
    int path_cursor;
    char* name;

    if((inumber = inumber_of_path(name_dir)) == 0) {
        PRINT_FATAL_ERROR("Not a valid path");
    }

    status = open_ifile(&fd, inumber);	
   	ffatal(!status, "erreur ouverture fichier %d", inumber);

    /* for each entries : recursive call or deletion */
    while (read_ifile(&fd, &entries, sizeof(struct entry_s)) > 0) {
        if (entries.inumber != 0) {
            read_inode(entries.inumber, &inode);
           if (inode.inode_type == FILE_DIRECTORY) {
                /* building the path for the recursive call */
                printf("\n");
           } else
               if(del_entry(inumber, entries.name) == -1)
                   PRINT_FATAL_ERROR("Unable to delete file");
        }
    }
    close_ifile(&fd);

    /* deletion of the entries in the parent directory */
    path_cursor = strlen(name_dir)-1;

    while(path_cursor >= 0 && name_dir[path_cursor] != '/') {
        path_cursor--;
    }

    if(path_cursor >= 0 && name_dir[path_cursor+1] != '\0') {
        int idir;
        char min_name[2];

        name = name_dir+path_cursor+1;

        if(path_cursor == 0) {
            min_name[0] = '/';
            min_name[1] = '\0';
            name_dir = min_name;
        }
        else {
            name_dir[path_cursor] = '\0';
        }
        
        if((idir = inumber_of_path(name_dir)) == 0) {
            PRINT_FATAL_ERROR("Not a valid path");
        }

        if(del_entry(idir, name) == -1) {
            PRINT_FATAL_ERROR("Unable to delete file");
        }
        
    }
    else
        PRINT_FATAL_ERROR("Not a valid path");

	return EXIT_SUCCESS;
}

static void
usage(const char *prgm)
{
    fprintf(stderr, "[%s] usage:\n\t"
            "%s <absolute_path>\n", prgm, prgm);
    exit(EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
    if (argc != 2)
        usage(argv[0]);

    mount();

    ddir(argv[1]);
    
    umount();

    exit(EXIT_SUCCESS);         
}
