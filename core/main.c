#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "include/hardware.h"
#include "config_hw.h"

#define MAX_PATH 64

static void empty_it(){
    return;
}

static void compute() {
    unsigned int i, j = 0;
    unsigned int n_core;
    n_core = _in(CORE_ID);
    while (1) {
    	printf("compute core #%d\n", n_core);
	    for (i = 0 ; i < 1073741824 ; i++)
	    	i = i;
	}
}

static void timer_it() {
	unsigned int n_core = _in(CORE_ID);
	printf("n_core:%d\n",n_core);
    _out(TIMER_ALARM,0xFFFFFFFF-100);
}

int main() {

	unsigned int i;

    /* init hardware */
    if(init_hardware("etc/core.ini") == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interrupt handlers */
    for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_it;

    /* fonction init */
	IRQVECTOR[0] = compute;

    /* handler du timer */
	IRQVECTOR[TIMER_IRQ] = timer_it;

    /* lance 3 core (0x7=111) */
	_out(CORE_STATUS, 0x7);

    /* map l'handler TIMER pour les cores 1 et 2 (valeur de i) */
    for(i = 1; i < 3 ; i++)
		_out(CORE_IRQMAPPER+i, 1 << TIMER_IRQ);

    /* init l'horloge */
	_out(TIMER_PARAM, 128+64+32+8);
    _out(TIMER_ALARM, 0xFFFFFFFF-100);

    while (1) ;

	return 0;
}