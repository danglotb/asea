/* ------------------------------
   $Id: mbr.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   MBR definition and manipualtion.
   Philippe Marquet, october 2002
   
*/

#ifndef _MBR_H_
#define _MBR_H_

#define MAX_VOLUME		8

#include "tools.h"
#include "drive.h"
#include "utility.h"
#include "ctype.h"

enum vol_type_e {
    base,
    annex,
    other
};

struct vol_descr_s {
    unsigned int vol_first_cylinder;
    unsigned int vol_first_sector;
    unsigned int vol_nb_sector;
    enum vol_type_e vol_type;
};

struct mbr_descr_s {
    unsigned int _magic_number; /* MBR_MAGIC */
    unsigned int mbr_nb_vol; 
    struct vol_descr_s mbr_vol[MAX_VOLUME]; /* first mbr_n_vol are in used */
};

typedef struct mbr_descr_s mbr_t;

mbr_t mbr;

/*
 * Convert couple of (Volume, Block) to Cynlinder
 */
#define BLOCK_TO_CYLINDER(VOL, BLOCK) mbr.mbr_vol[VOL].vol_first_cylinder+(mbr.mbr_vol[VOL].vol_first_sector+BLOCK)/NB_SECTOR
/*
 * Convert couple of (Volume, Block) to Sector
 */
#define BLOCK_TO_SECTOR(VOL, BLOCK) (mbr.mbr_vol[VOL].vol_first_sector+BLOCK)%NB_SECTOR

/* return RETURN_FAILURE in case of a non preexisting mbr */
void load_mbr();

void save_mbr();

void write_bloc(unsigned int volume, unsigned int block, const unsigned char* buffer);
void read_bloc(unsigned int volume, unsigned int block, unsigned char* buffer);

void format_vol(unsigned int volume);

char char_of_vol_type(enum vol_type_e);
enum vol_type_e vol_type_of_char(char);

#endif
