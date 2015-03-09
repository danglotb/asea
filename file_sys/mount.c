/* ------------------------------
   $Id: mount.c,v 1.2 2009/11/17 16:20:54 marquet Exp $
   ------------------------------------------------------------

   Initialization and finalization
   Philippe Marquet, Nov 2009
   
*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include "hardware.h"
#include "config.h"
#include "tools.h"
#include "hardware_config.h"
#include "mbr.h"
#include "manage_ctx.h"

/* load super bloc of the $CURRENT_VOLUME
   set current_volume accordingly */

extern struct pool_ctx_s *pool [NUMBER_CORE];
extern struct ctx_s *head [NUMBER_CORE];
extern struct ctx_s *current_ctx[NUMBER_CORE];
extern int current_vol;
extern int load_super(unsigned int);
extern void save_super();

void
load_current_volume (int fatal_e)
{
    char* current_volume_str;
    int status;
    
    current_volume_str = getenv("CURRENT_VOLUME");
    ffatal(current_volume_str != NULL, "no definition of $CURRENT_VOLUME"); 

    errno = 0;
    current_vol = strtol(current_volume_str, NULL, 10);
    ffatal(!errno, "bad value of $CURRENT_VOLUME %s", current_volume_str);
    
    status = load_super(current_vol);
	if(fatal_e) {
   		ffatal(!status, "unable to load super of vol %d", current_vol);
	}
}

/* return hw_config filename */
char *
get_hw_config ()
{
    char* hw_config;

    hw_config = getenv("HW_CONFIG");
    return hw_config ? hw_config : DEFAULT_HW_CONFIG;
}

static void
empty_it()
{
    return;
}

static void
timer_it() {
   _out(TIMER_ALARM,0xFFFFFFFF-100);
   _yield();
}

/*static void hda_irq(){
    hda_end_request();
}*/

static void init_loop(void * args) {
    while(1) {;}
}

static void init() {
    start_sched();
    _yield();
}

void boot() {
    char *hw_config;
    int status, i; 

    /* Hardware initialization */
    hw_config = get_hw_config();
    status = init_hardware(hw_config);
    ffatal(status, "error in hardware initialization with %s\n", hw_config);

    check_hda();

    for(i=0; i<16; i++)
        IRQVECTOR[i] = empty_it;

    IRQVECTOR[0] = init;

    IRQVECTOR[TIMER_IRQ] = timer_it;

    _out(CORE_STATUS, 0x7);

    _out(CORE_IRQMAPPER, 1 << HDA_IRQ);

    for(i = 1; i < 3 ; i++) {
        _out(CORE_IRQMAPPER+i, 1 << TIMER_IRQ);
    }

    _out(TIMER_PARAM, 128+64+32+8);
    _out(TIMER_ALARM, 0xFFFFFFFF-100);

    _out(CORE_IRQMAPPER, 1 << HDA_IRQ);

    _mask(0);

}

void init_multicore() {

    int i;
    for (i = 0 ; i < 4 ; i++) {
        head[i] = NULL;
        current_ctx[i] = NULL;
    }

    create_ctx(16634, init_loop, NULL, 1);
    create_ctx(16634, init_loop, NULL, 2);

    /* init hardware */
    if(init_hardware("core.ini") == 0) {
        fprintf(stderr, "Error in hardware initialization\n");
        exit(EXIT_FAILURE);
    }

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

    _mask(0);
}

/* ------------------------------
   Initialization and finalization fucntions
   ------------------------------------------------------------*/
void mount(){
    sem_init(&lock_disk, 1);
    /* Load MBR and current volume */
    load_mbr();
    load_current_volume(1);
}

void
umount()
{
    /* save current volume super bloc */
    save_super();

    /* bye */
}
 

