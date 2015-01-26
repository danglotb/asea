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
#include "../scheduler/manage_ctx.h"

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

static void tamer() {
  printf("TAMER\n");
}

/* ------------------------------
   Initialization and finalization fucntions
   ------------------------------------------------------------*/
void
mount()
{
    char *hw_config;
    int status, i; 


    /* Hardware initialization */
    hw_config = get_hw_config();
    status = init_hardware(hw_config);
    ffatal(status, "error in hardware initialization with %s\n", hw_config);

    check_hda();

    /* Interrupt handlers */
    for(i=0; i<16; i++) {
	     if (i != 2) 
        IRQVECTOR[i] = emptyIT;
    }

    IRQVECTOR[HDA_IRQ] = tamer;
    
    /* Allows all IT */
    _mask(1);

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
 

