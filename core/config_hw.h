#ifndef CONFIG_HW_H
#define CONFIG_HW_H

#define HDA_CMDREG 0x3F6
#define HDA_DATAREGS 0x110
#define HDA_IRQ 14

/* horloge */
#define TIMER_CLOCK	0xF0
#define TIMER_PARAM 0xF4
#define TIMER_ALARM 0xF8
#define TIMER_IRQ 2

/* Core */
#define CORE_STATUS 0x80
#define CORE_IRQMAPPER 0x82
#define CORE_ID 0x126

#define NUMBER_CORE 3

#endif