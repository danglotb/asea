#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dir.h"
#include "mount.h"
#include <unistd.h>
#include <math.h>

#include "manage_ctx.h"

#define MAX_PATH 64
#define STACK_SIZE 16384

typedef void (fun_cmd_t) (unsigned int, char* []);
typedef void (fun_loop_t) (void);

/* ------------------------------
   command list
   ------------------------------------------------------------*/
struct _cmd {
    char *name;
    void (*fun) (unsigned int argc, char *argv[]);
    char *comment;
};

struct proxy_loop {
    fun_loop_t *fun;
};

struct proxy_cmd {
    fun_cmd_t *fun;
    unsigned int argc;
    char **argv;
};

static char *current_directory;
struct proxy_loop *loop_call;

static void ls(unsigned int argc, char *argv[]);
static void cd(unsigned int argc, char *argv[]);
static void mkdir(unsigned int argc, char *argv[]);
static void compute(unsigned int argc, char *argv[]);
static void help(unsigned int argc, char *argv[]);
static void do_exit(unsigned int argc, char *argv[]);
static void none(unsigned int argc, char *argv[]);

static void proxy_cmd(void *);

static struct _cmd commands [] = {
    {"ls", ls, 	"affiche le contenu d'un repertoire"},
    {"cd", cd,  "change de repertoire courrant"},
    {"mkdir", mkdir, "cree un nouveau repertoire"},
    {"compute", compute , "fait un calcul tres compliqué"},
    {"exit", do_exit,	"exit the prompt"},
    {"help", help,	"display this help"},
    {0, none, "unknown command, try help"}
} ;

/* ------------------------------
   dialog and execute 
   ------------------------------------------------------------*/

static void execute(unsigned int argc, char **argv) {
    struct _cmd *c = commands;
    while (c->name && strcmp (argv[0], c->name))
        c++;
    if(argv[argc][0] == '&'){
        struct proxy_cmd *cmd;
        cmd = (struct proxy_cmd *)malloc(sizeof(struct proxy_cmd));
        cmd->fun = (*c->fun);
        cmd->argc = argc--;
        cmd->argv = argv;
        printf("%s\n", argv[1]);
        create_ctx(STACK_SIZE, proxy_cmd, cmd, 1);
    } else {
        (*c->fun)(argc, argv+1);
    }
}

static void loop(void) {
    char name[MAX_PATH];
    int error;
    unsigned int i, cpt;
    unsigned int argc;
    char *argv[MAX_PATH];
    argv[0] = (char *)malloc(sizeof(char *)*10);
    while (1){
        memset(name, 0, sizeof(char)*MAX_PATH);
        printf("%s> ", current_directory);
        fflush(stdout);
        error = read(STDIN_FILENO, name, MAX_PATH);
        PRINT_ASSERT_ERROR_MSG(error != -1, "error on read\n");
        fflush(stdin);
        i = 0;
        argc = 0;
        cpt = 0;
        while(name[i] != '\n'){
          /*  printf("name[%d] : %c\n", i, name[i]); */
            if(name[i] == ' '){
                argv[argc] = (char *)malloc(sizeof(char)*(i-cpt));
                strncpy(argv[argc], name+cpt, i-cpt);
            /*    printf("argc : %d\n", argc);
                printf("argv : %s\n", argv[argc]);
            */  argc++;
                cpt = i+1;

            }
            i++;
        }
        argv[argc] = (char *)malloc(sizeof(char)*(i-cpt));
        strncpy(argv[argc], name+cpt, i-cpt);
    /*  printf("argc : %d\n", argc);
        printf("argv : %s\n", argv[argc]);
    */  execute(argc, argv);
        for(i = 0; i < argc; i++){
            free(argv[i]);
        }
    }
}

/* commands */

/* change directory */

static void cd(unsigned int argc, char * argv[]) {
    char pathname[MAX_PATH];
    if (argc == 0) 
        strcpy(current_directory, "/");
    else if(argc == 1 && strcmp(argv[0], ".") == 0)
        return;
    else {
         if (argv[0][0] != ('/')) {  
            strcpy(pathname, current_directory);
            if(argc > 0) {
                strcat(pathname, argv[0]);
            }
        } else
                strcpy(pathname, argv[0]);
        if (inumber_of_path(pathname) == 0) {
           printf("Not a valid path\n");
           return;
        }
        strncpy(current_directory, pathname, MAX_PATH);
    }
}

/* list command actually only work on current directory */

static void ls(unsigned int argc, char *argv[]) {
    unsigned int inumber;
    char pathname[MAX_PATH];
/*  printf("current_directory : %s\n", current_directory); */

    if(strlen(current_directory) > MAX_PATH) {
        printf("Actual path is too long\n");
        return;
    }
    if (argc == 0 || strcmp(argv[0], ".") == 0) {
        strcpy(pathname, current_directory);
    } else {
        if (argv[0][0] != ('/')) {  
            strcpy(pathname, current_directory);
            if(argc > 0)
                strcat(pathname, argv[0]);
        } else
                strcpy(pathname, argv[0]);
    }
    inumber = inumber_of_path(pathname);
     if( inumber != 0) {
        file_desc_t fd;
        if(open_ifile(&fd, inumber) != RETURN_FAILURE) {
            struct entry_s entry;
            while(read_ifile(&fd, &entry, sizeof(struct entry_s)) > 0) {
                if(entry.inumber != 0)
                    printf("%s\n", entry.name);
            }
            close_ifile(&fd);
        }
    } else {
           return;
        }
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
         printf("Not a valid path\n");
}

/* compute function */

static void compute(unsigned int argc, char *argv[]) {
    unsigned int i;
    printf("Begin compute... \n");
    for (i = 0 ; i < 1663400000 ; i++)
        i = i;
    printf("End compute... \n");
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
    free(loop_call);
    exit(EXIT_SUCCESS);
}

/* if unmatched command */

static void none(unsigned int argc, char *argv[]) {
    printf ("unknown command, try help\n") ;
}


/* proxy function (void*) to any function */

static void proxy_cmd(void * proxy) {
    struct proxy_cmd *proxy_call = (struct proxy_cmd*)proxy;
    (proxy_call->fun)(proxy_call->argc, proxy_call->argv);
    free(proxy_call);
}

/* proxy function (void*) to loop function */

/*static void proxy_loop(void * proxy) {
    struct proxy_loop *proxy_call = (struct proxy_loop*)proxy;
    (proxy_call->fun)();
}*/

/* main */

int main(int argc, char **argv) {

//   boot();
//   mount();

    init_multicore();

    current_directory = (char*)malloc(sizeof(char)*MAX_PATH);
    strcpy(current_directory, "/");

    loop();

    do_exit(0, NULL);

    /* make gcc -W happy */
    exit(EXIT_SUCCESS);
}
