#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "include/hardware.h"

#include "manage_ctx.h"

#define MAX_PATH 64

typedef void (fun_cmd_t) (unsigned int, char* []);
typedef void (fun_loop_t) (void);

struct _cmd {
    char *name;
    void (*fun) (unsigned int argc, char *argv[]);
    char *comment;
};

struct proxy_cmd {
    fun_cmd_t *fun;
    unsigned int argc;
    char **argv;
};


extern struct ctx_s *head [4];
extern struct ctx_s *current_ctx[4];

static void compute(void * args);
static void compute2(void * args);
static void help(unsigned int argc, char *argv[]);
static void none(unsigned int argc, char *argv[]);

static void proxy_cmd(void *);

static struct _cmd commands [] = {
    {"compute", compute , "fait un calcul tres compliqué"},
    {"compute2", compute2 , "fait un calcul tres compliqué"},
    {"help", help,	"display this help"},
    {0, none, "unknown command, try help"}
} ;


static void empty_it(){
    return;
}

static void help(unsigned int argc, char *argv[]) {
    struct _cmd *c = commands;
    for (; c->name; c++) 
      printf ("%s\t-- %s\n", c->name, c->comment);
}

static void none(unsigned int argc, char *argv[]) {
    printf ("unknown command, try help\n") ;
}

static void compute(void * args) {
    unsigned int i;
    unsigned int n_core;
    n_core = _in(CORE_ID);
  	while(1){
    	printf("compute core #%d\n", n_core);
	    for (i = 0 ; i < 1663400000 ; i++)
	    	i = i;
	}
}

static void compute2(void * args) {
    unsigned int i;
    unsigned int n_core;
    n_core = _in(CORE_ID);
   while(1){
    	printf("compute 2 core #%d\n", n_core);
	    for (i = 0 ; i < 1663400000 ; i++)
	    	i = i;
	}
}

static void init_loop(void * args) {
	while(1) {;}
}

static void timer_it() {
/*	unsigned int n_core = _in(CORE_ID);
	printf("n_core:%d\n",n_core);
*/	_out(TIMER_ALARM,0xFFFFFFFF-100);
	_yield();

}

static void init() {
	int n_core = _in(CORE_ID);
	printf("%d active\n", n_core);
	start_sched();
}

static void execute(char * argv) {
    struct _cmd *c = commands;
    while (c->name && strcmp (argv, c->name))
        c++;
    printf("c->name");
   	
    /* must create with add_ctx_to_assigned_core */
   	//create_ctx(STACK_SIZE, proxy_cmd, cmd, 1);
}


static void proxy_cmd(void * proxy) {
    struct proxy_cmd *proxy_call = (struct proxy_cmd*)proxy;
    (proxy_call->fun)(proxy_call->argc, proxy_call->argv);
    free(proxy_call);
}

static void loop(void) {
    char cmd[MAX_PATH];
    int error;
    while (1){
        printf("$> ");
        error = read(STDIN_FILENO, cmd, MAX_PATH);
    	execute(cmd);
    }
}

int main() {

	unsigned int i;

	for (i = 0 ; i < 4 ; i++) {
		head[i] = NULL;
		current_ctx[i] = NULL;
	}

	create_ctx(16634, init_loop, NULL, 1);
	create_ctx(16634, compute, NULL, 1);
	create_ctx(16634, init_loop, NULL, 2);

    /* init hardware */
    if(init_hardware("etc/core.ini") == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interrupt handlers */
    for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_it;

	IRQVECTOR[0] = init;

	IRQVECTOR[TIMER_IRQ] = timer_it;

	_out(CORE_STATUS, 0x7);

    for(i = 1; i < 3 ; i++) {
		_out(CORE_IRQMAPPER+i, 1 << TIMER_IRQ);
	}

	_out(TIMER_PARAM, 128+64+32+8);
    _out(TIMER_ALARM, 0xFFFFFFFF-100);

    /* implements the loop */
   	loop();
	//init();
	return 0;
}