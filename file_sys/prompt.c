#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dir.h"
#include "mount.h"
#include <unistd.h>

#define MAX_PATH 64

/* ------------------------------
   command list
   ------------------------------------------------------------*/
struct _cmd {
    char *name;
    void (*fun) (unsigned int argc, char *argv[]);
    char *comment;
};

char *current_directory;

static void ls(unsigned int argc, char *argv[]);
static void cd(unsigned int argc, char *argv[]);
static void help(unsigned int argc, char *argv[]);
static void do_exit(unsigned int argc, char *argv[]);
static void none(unsigned int argc, char *argv[]);

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


static void execute(unsigned int argc, char *argv[]) {
    struct _cmd *c = commands; 
    while (c->name && strcmp (argv[0], c->name))
        c++;
    (*c->fun)(argc, argv+1);
}

static void loop(void) {
    char name[64];
    int error;
    unsigned int i, cpt;
    unsigned int argc;
    char *argv[64];
    argv[0] = (char *)malloc(sizeof(char *)*10);
    while (1){
        memset(name, 0, sizeof(char)*64);
        printf("> ");
        fflush(stdout);
        error = read(STDIN_FILENO, name, 64);
        if(error == -1)
            printf("YA UN PBLM %d \n", error);
        fflush(stdin);
        i = 0;
        argc = 0;
        cpt = 0;
        while(name[i] != '\0'){
            /*printf("name[%d] : %c\n", i, name[i]);*/
            if(name[i] == ' '){
                argv[argc] = (char *)malloc(sizeof(char *)*i-cpt);
                strncpy(argv[argc], name+cpt, i-cpt);
                /*printf("argc : %d\n", argc);
                printf("argv : %s\n", argv[argc]);*/
                argc++;
                cpt = i+1;

            }
            i++;
        }
        argv[argc] = (char *)malloc(sizeof(char *)*i-cpt);
        strncpy(argv[argc], name+cpt, i-cpt);
        /*printf("argc : %d\n", argc);
        printf("argv : %s\n", argv[argc]);*/
        argc++;
       execute(argc, argv);
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

static void ls(unsigned int argc, char *argv[]) {
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

static void help(unsigned int argc, char *argv[]) {
    struct _cmd *c = commands;
    for (; c->name; c++) 
    printf ("%s\t-- %s\n", c->name, c->comment);
}

/* exit command */

static void do_exit(unsigned int argc, char *argv[]) {
    umount();
    exit(EXIT_SUCCESS);
}

/* if unmatched command */

static void none(unsigned int argc, char *argv[]) {
    struct _cmd *c = commands;
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

    do_exit(0, NULL);

    /* make gcc -W happy */
    exit(EXIT_SUCCESS);
}