/* ------------------------------
   $Id: inode.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   Inode manipulation.
   Philippe Marquet, october 2002
   
*/

#ifndef _INODE_H_
#define _INODE_H_

#include "tools.h"

#include "bloc_allocator.h" /*	for BLOC_SIZE */
   
#define BLOC_SIZE       SECTOR_SIZE
#define DATA_BLOC_SIZE  BLOC_SIZE
#define NB_DIRECT_BLOC BLOC_SIZE/sizeof(int)-5*sizeof(int)
#define NB_INDIRECT_BLOC BLOC_SIZE/sizeof(int)
   
enum file_type_e {FILE_FILE, FILE_DIRECTORY, FILE_SPECIAL};

struct inode_s {
  enum file_type_e inode_type;
  unsigned int ind_size;
  unsigned int inode_direct_bloc[NB_DIRECT_BLOC];
  unsigned int inode_indirect_1;
  unsigned int inode_indirect_2;
};

/* a bloc full of zeros */
#define BLOC_NULL 0

/* number of bloc number in a bloc */
#define NBLOC_PER_BLOC ((BLOC_SIZE/sizeof(int)))

unsigned int create_inode(enum file_type_e type);
int delete_inode(unsigned int inumber);

void read_inode (unsigned int inumber, struct inode_s *inode);
void write_inode (unsigned int inumber, const struct inode_s *inode);

/* return the bloc index on the volume of a given bloc index in a
   file.  
   Return BLOC_NULL for a bloc full of zeros */ 
unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, bool_t do_allocate); 

/* allocate and return a bloc on the volume (in order to write in the
   file).
   This may imply indirect and d_indirect bloc creation.
   Return BLOC_NULL if no allocation was possible. */
/*unsigned int allocate_vbloc_of_fbloc(unsigned int inumber, unsigned int bloc);
*/
#endif
