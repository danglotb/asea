/* ------------------------------
   $Id: tmr.c 69 2007-05-11 11:54:15Z phm $
   ------------------------------------------------------------

   Program a timer

   Philippe Marquet, march 2007

   A minimal example of a program using the timer interface.
   
*/

#include <stdlib.h>
#include <stdio.h>
#include "hardware.h"
#include "hw_config.h"

static void
empty_it(void)
{
    return;
}

static void
timer_it() {
    static int tick = 0; 
    fprintf(stderr, "hello from IT %d\n", ++tick);
    _out(TIMER_ALARM,0xFFFFFFFE);
}

int
main(int argc, char **argv)
{
    unsigned int i;
    
    /* init hardware */
    if (init_hardware(HARDWARE_INI) == 0) {
	fprintf(stderr, "Error in hardware initialization\n");
	exit(EXIT_FAILURE);
    }
    
    /* dummy interrupt handlers */
    for (i=0; i<16; i++)
	IRQVECTOR[i] = empty_it;

    /* program timer */
    IRQVECTOR[TIMER_IRQ] = timer_it;    
    _out(TIMER_PARAM,128+64+32+8); /* reset + alarm on + 8 tick / alarm */
    _out(TIMER_ALARM,0xFFFFFFFE);  /* alarm at next tick (at 0xFFFFFFFF) */

   /* allows all IT */
    _mask(1);
    
    /* count for a while... */
    for (i=0; i<(1<<28); i++)
	;

    /* and exit! */
    exit(EXIT_SUCCESS);
}
