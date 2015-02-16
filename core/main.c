#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/hardware.h"

#include "manage_ctx.h"

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


static void
timer_it() {
	unsigned int n_core;
    n_core = _in(CORE_ID);
	printf("n_core :%d\n",n_core);
    _out(TIMER_ALARM,0xFFFFFFFF-100);
  //_yield();
}


static void init_h(){
	int n_core = _in(CORE_ID);
	/*switch(n_core){
		case 1:
			compute(NULL);
			break;
		case 2:
			compute(NULL);
			break;
		case 3:
			compute(NULL);
			break;
		case 4:
			compute(NULL);
			break;
		case 5:
			compute(NULL);
			break;
		case 6:
			compute(NULL);
			break;
		case 7:
			compute(NULL);
			break;
	}*/
	_mask(0);
	printf("%d active\n", n_core);
	/*start_sched();*/
	while (1) {;}
}

static void init(void * args) {
	int n_core = _in(CORE_ID);
	printf("%d active\n", n_core);
	_mask(0);
	while (1) {;}
}


int main(){

	unsigned int i;
	int core_status;

	create_ctx(16634, init, NULL,1);
	create_ctx(16634, compute, NULL,1);
	create_ctx(16634, init, NULL,2);
	create_ctx(16634, compute, NULL,2);
	create_ctx(16634, init, NULL,3);
	create_ctx(16634, compute, NULL,3);

    /* init hardware */
    if(init_hardware("etc/core.ini") == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

	

    /* Interrupt handlers */
    for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_it;

	IRQVECTOR[0] = init_h;

	_out(TIMER_PARAM, 128+64+32+8);
    _out(TIMER_ALARM,0xFFFFFFFF-100);

	IRQVECTOR[TIMER_IRQ] = timer_it;

	_out(CORE_STATUS, 0xF);

	for(i = 0; i < 4 ; i++) {
		_out(CORE_IRQMAPPER+i, 0x1 << TIMER_IRQ);
	}

	_mask(0);

	// IRQVECTOR[0] = timer;
	// _out(CORE_IRQMAPPER, 0);
	// _out(CORE_STATUS, 0x3);

 	core_status = _in(CORE_STATUS);
	printf("core_status : %d\n", core_status);
	start_sched();
	compute(NULL);
	return 0;
}