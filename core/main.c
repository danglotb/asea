#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "include/hardware.h"


#define CORE_STATUS 0x80
#define CORE_IRQMAPPER 0x82
#define CORE_ID 0x126

#define TIMER_CLOCK	0xF0
#define TIMER_PARAM 0xF4
#define TIMER_ALARM 0xF8
#define TIMER_IRQ 2   

static void
empty_it()
{
    return;
}

static void compute() {
    unsigned int i;
    unsigned int n_core;
    n_core = _in(CORE_ID);
    while(1){
	    printf("compute core #%d\n", n_core);
	    for (i = 0 ; i < 16634000 ; i++)
	        i = i;
	 }
}

static void
timer_it() {
    _out(TIMER_ALARM,0xFFFFFFFD);
}

static void timer(){
    unsigned int i;
    unsigned int n_core;
    n_core = _in(CORE_ID);
    while(1){
	    printf("timer core #%d\n", n_core);
	    for (i = 0 ; i < 16634000 ; i++)
	        i = i;
	 }
}

int main(){

	unsigned int i;
    /* init hardware */
    if(init_hardware("etc/core.ini") == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interreupt handlers */
    for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_it;

	IRQVECTOR[TIMER_IRQ] = timer_it;
	IRQVECTOR[0] = compute;
	IRQVECTOR[1] = timer;
	_out(CORE_IRQMAPPER+1, 0x1 << TIMER_IRQ);
	for(i = 2; i < 4; i++){
		_out(CORE_IRQMAPPER+i, 0);
	}
	_out(CORE_STATUS, 0xF);
	_mask(0);
	// IRQVECTOR[0] = timer;
	// _out(CORE_IRQMAPPER, 0);
	// _out(CORE_STATUS, 0x3);
	compute();
	return 0;
}