#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "include/hardware.h"

#include "manage_ctx.h"


extern struct ctx_s *head [NUMBER_CORE];
extern struct ctx_s *current_ctx[NUMBER_CORE];

/* fonction vide pour init le IRQVector */
static void empty_it(){
    return;
}

/* j n'est pas utile puisque l'arret des ctx provoque une seg fault */

/* fonction compute 1 */
static void compute(void * args) {
    unsigned int i, j = 0;
    unsigned int n_core;
    n_core = _in(CORE_ID);
  	while(1){
    	printf("compute core #%d\n", n_core);
	    for (i = 0 ; i < 166340000 ; i++)
	    	i = i;
        j++;
	}
    printf("end - compute core #%d\n", n_core);
}

/* fonction compute 2 */
static void compute2(void * args) {
    unsigned int i, j = 0;
    unsigned int n_core;
    n_core = _in(CORE_ID);
   while(1){
    	printf("compute 2 core #%d\n", n_core);
	    for (i = 0 ; i < 166340000 ; i++)
	    	i = i;
        j++;
	}
    printf("end - compute 2 core #%d\n", n_core);
}

/* fonction loop vide qui est le point de chute des cores (ne fonctionne pas) */
static void init_loop(void * args) {
	while(1) {}
}

/* handler sur l'horloge appel yield pour changer de contexte */
static void timer_it() {
	unsigned int n_core = _in(CORE_ID);
	printf("n_core:%d\n",n_core);
    /* seul 1 core a besoin de reinit l'alarme */
    if (n_core == 1)
        _out(TIMER_ALARM,0xFFFFFFFF-100);
	_yield();

}

/* fonction d'initialise du scheduler */
static void init() {
	int n_core = _in(CORE_ID);
	printf("%d active\n", n_core);
	start_sched();
}

/* fonction main */
int main() {

	unsigned int i;

    /* init les rings de ctx */
	for (i = 0 ; i < 4 ; i++) {
		head[i] = NULL;
		current_ctx[i] = NULL;
	}

    /* ajoute 3 ctx aux rings des cores 1 et 2 */
	create_ctx(16634, init_loop, NULL, 1);
	create_ctx(16634, init_loop, NULL, 2);
    create_ctx(16634, compute, NULL, 1);
    create_ctx(16634, compute, NULL, 2);
    create_ctx(16634, compute2, NULL, 1);
    create_ctx(16634, compute2, NULL, 2);

    /* init hardware */
    if(init_hardware("etc/core.ini") == 0) {
		fprintf(stderr, "Error in hardware initialization\n");
		exit(EXIT_FAILURE);
    }

    /* Interrupt handlers */
    for(i=0; i<16; i++)
		IRQVECTOR[i] = empty_it;

    /* fonction init */
	IRQVECTOR[0] = init;

    /* handler du timer */
	IRQVECTOR[TIMER_IRQ] = timer_it;

    /* lance 3 core (0x7=111) */
	_out(CORE_STATUS, 0x7);

    /* map l'handler TIMER pour les cores 1 et 2 (valeur de i) */
    for(i = 1; i < 3 ; i++) {
		_out(CORE_IRQMAPPER+i, 1 << TIMER_IRQ);
	}

    /* init l'horloge */
	_out(TIMER_PARAM, 128+64+32+8);
    _out(TIMER_ALARM, 0xFFFFFFFF-100);

    /* lance le core 0 dans une boucle infini */
    init_loop(NULL);

	return 0;
}