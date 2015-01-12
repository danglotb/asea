#ifndef _DRIVE_H
#define _DRIVE_H

#include <hardware.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utility.h"

#define SECTOR_SIZE 512
#define NB_CYLINDER 16
#define NB_SECTOR 16


/*
 *	Check HDA hardware configuration
 */
void check_hda();

/*
 * Read one sector
 */
void read_sector(unsigned int cylinder, unsigned int sector, unsigned char* buffer);

/*
 *	Read n bytes in sector
 */
void read_sector_n(unsigned int cylinder, unsigned int sector, unsigned char* buffer, unsigned int n);

/*
 *	Write one sector
 */
void write_sector(unsigned int cylinder, unsigned int sector, const unsigned char* buffer);

/*
 *	Write n bytes in sector
 */
void write_sector_n(unsigned int cylinder, unsigned int sector, const unsigned char* buffer, unsigned int n);

/*
 *	Fromat n sector with value
 */
void format_sector(unsigned int cylinder, unsigned int sector, unsigned int n_sector, unsigned int value);

#endif
