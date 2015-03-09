#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/hardware.h"

#include "manage_ctx.h"

extern struct ctx_s *head [4];
extern struct ctx_s *current_ctx[4];

static void
empty_it()
{
    return;
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
}




int main() {

	unsigned int i;

	for (i = 0 ; i < 4 ; i++) {
		head[i] = NULL;
		current_ctx[i] = NULL;
	}

	create_ctx(16634, compute2, NULL,0);
	create_ctx(16634, compute, NULL,0);
	create_ctx(16634, compute2, NULL,1);
	create_ctx(16634, compute, NULL,1);
	create_ctx(16634, compute2, NULL,2);
	create_ctx(16634, compute, NULL,2);

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

    while(1){;}
	//init();
	return 0;
}