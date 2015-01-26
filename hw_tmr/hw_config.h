/* ------------------------------
   $Id: hw_config.h 86 2007-06-01 14:34:35Z skaczmarek $
   ------------------------------------------------------------

   Fichier de configuration des acces au materiel

   Philippe Marquet, march 2007

   Code au niveau applicatif la description du materiel qui est fournie
   par hardware.ini
   
*/

#ifndef _HW_CONFIG_H_
#define _HW_CONFIG_H_

#define HARDWARE_INI	"hardware.ini"

/* Horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM     0xF4
#define TIMER_ALARM     0xF8
#define TIMER_IRQ	2   

#endif
