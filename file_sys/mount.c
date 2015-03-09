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
emptyIT()
{
    return;
}

static void
timer_it() {
    _out(TIMER_ALARM,0xFFFFFFFD);
    _yield();
}

/*static void hda_irq(){
    hda_end_request();
}
*/
static void compute() {
    unsigned int i;
    int core = _in(CORE_ID);
    printf("Begin compute... %d\n", core);
    for (i = 0 ; i < 16634000 ; i++)
        i = i;
    printf("End compute... \n");
}


static void init() {
    while (1){ compute();}
}

void boot() {
    char *hw_config;
    int status, i; 


    /* Hardware initialization */
    hw_config = get_hw_config();
    status = init_hardware(hw_config);
    ffatal(status, "error in hardware initialization with %s\n", hw_config);

    check_hda();

    _out(CORE_STATUS, 0xF);

    /* Interrupt handlers */
    for(i=0; i<16; i++)
        IRQVECTOR[i] = emptyIT;

    IRQVECTOR[TIMER_IRQ] = timer_it;
    IRQVECTOR[0] = init;

  
    for(i = 1; i < 3; i++)
          _out(CORE_IRQMAPPER+i, 0x1 << TIMER_IRQ);

    _out(TIMER_PARAM,128+64+32+8); /* reset + alarm on + 8 tick / alarm */
    _out(TIMER_ALARM,0xFFFFFFFD);   /* alarm at next tick (at 0xFFFFFFFF) */

    /* Allows all IT */
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
 

