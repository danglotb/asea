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
static void mkdir(unsigned int argc, char *argv[]);
static void help(unsigned int argc, char *argv[]);
static void do_exit(unsigned int argc, char *argv[]);
static void none(unsigned int argc, char *argv[]);

static struct _cmd commands [] = {
    {"ls", ls, 	"affiche le contenu d'un repertoire"},
    {"cd", cd,  "change de repertoire courrant"},
    {"mkdir", mkdir, "cree un nouveau repertoire"},
    {"exit", do_exit,	"exit the prompt"},
    {"help", help,	"display this help"},
    {0, none, 		"unknown command, try help"}
} ;

/* ------------------------------
   dialog and execute 
   ------------------------------------------------------------*/

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
        printf("%s> ", current_directory);
        fflush(stdout);
        error = read(STDIN_FILENO, name, 64);
        if(error == -1)
            printf("YA UN PBLM %d \n", error);
        fflush(stdin);
        i = 0;
        argc = 0;
        cpt = 0;
        while(name[i] != '\n'){
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
       execute(argc, argv);
    }
}

/* commands */

/* change directory */

static void cd(unsigned int argc, char * argv[]) {
    if (argc == 0)
        current_directory = "/";
    else {
        if (inumber_of_path(argv[0]) == 0) {
           printf("Not a valid path\n");
           return;
        }
        current_directory = argv[0];
    }
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
    } else
         PRINT_FATAL_ERROR("Not a valid path");
}

/* create a directory */

static void mkdir(unsigned int argc, char *argv[]) {

    int path_cursor;
    char* name;

    if(argc < 1) {
        printf("./%s <directory_name>\n", argv[0]);
        return;
    }

    path_cursor = strlen(argv[0])-1;

    while(path_cursor >= 0 && argv[0][path_cursor] != '/') {
        path_cursor--;
    }


    if(path_cursor >= 0 && argv[0][path_cursor+1] != '\0') {
        unsigned int inumber;
        char min_name[2];

        name = argv[0]+path_cursor+1;

        if(path_cursor == 0) {
            min_name[0] = '/';
            min_name[1] = '\0';
            argv[0] = min_name;
        }
        else {
            argv[0][path_cursor] = '\0';
        }
        
        if((inumber = inumber_of_path(argv[0])) == 0) {
            PRINT_FATAL_ERROR("Not a valid path");
        }

        if(add_entry(inumber, name, FILE_DIRECTORY) == -1) {
            PRINT_FATAL_ERROR("Unable to create directory");
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
