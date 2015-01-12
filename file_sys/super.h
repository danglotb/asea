/* ------------------------------
   $Id: super.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   Super bloc management.
   Philippe Marquet, october 2002
   
*/

#ifndef _SUPER_H_
#define _SUPER_H_

/* the index of a bloc full of zeroes */
#define BLOC_NULL	0

struct super_s {
    unsigned int super_magic; /* SUPER_MAGIC */
    unsigned int super_root; 
    unsigned int super_free_size; /* number of blocs */
    unsigned int super_first_free;
};

extern struct super_s super;
unsigned int current_volume; 

void init_super(unsigned int vol);
void display_super(unsigned int vol);

/* return RETURN_FAILURE if the super bloc was not valid */
int load_super(unsigned int vol);

void save_super();

/* return RETURN_FAILURE if a super bloc is not loaded */
int super_loaded();

/* return BLOC_NULL in case of failure */
unsigned int new_bloc();

void free_bloc(unsigned int bloc);

#endif
