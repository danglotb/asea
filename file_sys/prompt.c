#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dir.h"
#include "mount.h"

#define MAX_PATH 64

/* ------------------------------
   command list
   ------------------------------------------------------------*/
struct _cmd {
    char *name;
    void (*fun) (struct _cmd *c);
    char *comment;
};

char *current_directory;

static void ls(struct _cmd *c);
static void cd(struct _cmd *c);
static void help(struct _cmd *c);
static void do_exit(struct _cmd *c);
static void none(struct _cmd *c);

static struct _cmd commands [] = {
    {"ls", ls, 	"affiche le contenu d'un repertoire"},
    {"cd", cd,  "change de repertoire courrant"},
    {"exit", do_exit,	"exit the prompt"},
    {"help", help,	"display this help"},
    {0, none, 		"unknown command, try help"}
} ;

/* ------------------------------
   dialog and execute 
   ------------------------------------------------------------*/
/*
static void execute(const char *name) {
    struct _cmd *c = commands; 
  
    while (c->name && strcmp (name, c->name))
    c++;
    (*c->fun)(c);
}

static void loop(void) {
    char name[64];
    while (printf("> "), scanf("%s", name) == 1){
       execute(name) ;
    }
}
*/



static void execute(char *name) {
    struct _cmd *c = commands; 
    const char* delimiter = " ";
    char *args = strtok(name, delimiter);
    while(args != NULL){
        printf("%s\n", args);
        args = strtok(NULL, delimiter);
    }
    while (c->name && strcmp (name, c->name))
	c++;
    (*c->fun)(c);
}

static void loop(void) {
    char name[64];
    while (1){
        printf("> ");
        scanf("%s[^\n]", name);
        execute(name);
    }
}

/* commands */

/* change directory */

static void cd(unsigned int argc, char * argv[]) {
    unsigned int cursor = 0, partial_cursor = 0;
    unsigned int parent_inumber = current_super_bloc.sb_inode_root;
    char path[MAX_PATH];
    PRINT_ASSERT_ERROR_MSG(arg =< 0, "Bad argument for cd");
    while (argv[1][cursor] != '\0') {
        if (argv[1][cursor] == '/') {
            check(path, &parent_inumber);
            /* reinit for the next checking */
            cursor++
            memset(path, 0, partial_cursor+1)
            partial_cursor = 0;
        }
        path[partial_cursor++] = argv[1][cursor++];
    }
    current_directory = path;   
}

/* check if it is a valid path 
    update the inumber with  the new inumbre_directory */

static int check(char *path, unsigned int *inumber) {
    file_desc_t fd;
    if(open_ifile(&fd, inumber) != RETURN_FAILURE) {
            *inumber = find_entry(&fd, path);
            PRINT_ASSERT_ERROR_MSG(*inumber > -1, "Wrong path");
            close_ifile(&fd);
    } else
         PRINT_FATAL_ERROR("Not a valid path");
}

/* list command actually only work on current directory */

static void ls(struct _cmd *c) {
    unsigned int inumber;
     if((inumber = inumber_of_path(current_directory) != 0)) {
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
}

/* print help */

static void help(struct _cmd *dummy) {
    struct _cmd *c = commands;
    for (; c->name; c++) 
    printf ("%s\t-- %s\n", c->name, c->comment);
}

/* exit command */

static void do_exit(struct _cmd *c) {
    umount();
    exit(EXIT_SUCCESS);
}

/* if unmatched command */

static void none(struct _cmd *c) {
    printf ("%s\n", c->comment) ;
}

/* main */

int
main(int argc, char **argv)
{

    mount();

    current_directory = "/";

    /* dialog with user */ 
    loop();

    do_exit(NULL);

    /* make gcc -W happy */
    exit(EXIT_SUCCESS);
}
