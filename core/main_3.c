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
    void (*fun) (void*);
    char *comment;
};

struct proxy_cmd {
    fun_cmd_t *fun;
    unsigned int argc;
    char **argv;
};


extern struct ctx_s *head [NUMBER_CORE];
extern struct ctx_s *current_ctx[NUMBER_CORE];
extern weight_ctx_to_core_s assigned_weight[NUMBER_CORE];

static void compute(void*);
static void compute2(void*);
static void ps(void*);
static void help(void*);
static void none(void*);

static void proxy_cmd(void *);

static struct _cmd commands [] = {
    {"compute\n", compute, "calcul fini"},
    {"compute2\n", compute2, "calcul infini"},
    {"ps\n", ps, "affiche la charge sur chaque core"},
    {"help\n", help, "display this help"},
    {0, none, "unknown command, try help"}
};

static void help(void* args) {
    struct _cmd *c = commands;
    for (; c->name ; c++) 
      printf ("%s\t-- %s\n", c->name, c->comment);
}

static void none(void* args) {
    printf ("unknown command, try help\n") ;
}


static void empty_it(){
    return;
}

static void ps(void* args) {
    unsigned int i;
    for(i = 0 ; i < NUMBER_CORE ; i++) {
        printf("charge pour #%d : %d\n", i, assigned_weight[i].weight);
    }
}

static void compute(void* args) {
    unsigned int i, j = 0;
    unsigned int n_core;
    n_core = _in(CORE_ID);
    while (j<2) {
    	printf("compute core #%d\n", n_core);
	    for (i = 0 ; i < 166340000 ; i++)
	    	i = i;
        j++;
	}
    printf("end - compute core #%d\n", n_core);
}

static void compute2(void* args) {
    unsigned int i, j = 0;
    unsigned int n_core;
    n_core = _in(CORE_ID);
    while(j<2) {
   /*while(1){*/
    	printf("compute 2 core #%d\n", n_core);
	    for (i = 0 ; i < 166340000 ; i++)
	    	i = i;
        j++;
	}
    printf("end - compute 2 core #%d\n", n_core);
}

static void init_loop(void * args) {
	while(1) {}
}

static void timer_it() {
	unsigned int n_core = _in(CORE_ID);
	printf("n_core:%d\n",n_core);
    _out(TIMER_ALARM,0xFFFFFFFF-100);
	_yield();
}

static void init() {
	int n_core = _in(CORE_ID);
	printf("%d active\n", n_core);
    start_sched();
    _yield();
}


static void execute(char * argv) {
    struct _cmd *c = commands;
    while (c->name && strcmp(argv,c->name))
          c++;

    c->fun(NULL);

    /* add_ctx_to_assigned_core(c->fun, 1); */   


    /* add a context to the ring 0 : others core will take it 
    create_ctx(16634, c->fun(NULL), NULL, 0);*/
}  


static void proxy_cmd(void * proxy) {
    struct proxy_cmd *proxy_call = (struct proxy_cmd*)proxy;
    (proxy_call->fun)(proxy_call->argc, proxy_call->argv);
    free(proxy_call);
}

static void loop(void) {
    char cmd[MAX_PATH];
    while (1){
        memset(cmd, 0, sizeof(char)*MAX_PATH);
        printf("$> ");
        fflush(0);
        read(STDIN_FILENO, cmd, MAX_PATH);
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

    loop();

	return 0;
}