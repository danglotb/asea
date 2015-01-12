/* ------------------------------
   $Id: ifile.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   File manipulation. 
   Basic version; manipulate files via their inode number. 
   Philippe Marquet, october 2002
   
*/

#ifndef _IFILE_H_
#define _IFILE_H_

#include "tools.h"
/*#include "cfile.h" */
#include "inode.h"

#define READ_EOF    -2
#if READ_EOF >= 0
#  error "READ_EOF must be negative"
#endif


struct file_desc_s {
    unsigned int fds_inumber;   /* inode number of the file */
    unsigned int fds_size;      /* file size in char */
    unsigned int fds_pos;       /* cursor in the file, in char */
    unsigned char fds_buf[DATA_BLOC_SIZE]; /* memory copy of bloc at fds_pos */
    char fds_dirty;             /* buffer write back is needed */
};

typedef struct file_desc_s file_desc_t; 

/*------------------------------
  File creation and deletion
  ------------------------------------------------------------*/

/* return the inumber of the newly created file, 0 if error. */
unsigned int create_ifile(enum file_type_e type);
/* free all blocs associated to the inode */
int delete_ifile(unsigned int inumber);

/*------------------------------
  File opening
  ------------------------------------------------------------*/
int open_ifile(file_desc_t *fd, unsigned int inumber);

/*------------------------------
  Functions copied from sfile. 
  ------------------------------------------------------------*/

/*#include "sfile.h" */

/* no inline keyword in ANSI, sorry */
void close_ifile(file_desc_t *fd);
void flush_ifile(file_desc_t *fd);
void seek_ifile(file_desc_t *fd, int r_offset);  
void seek2_ifile(file_desc_t *fd, int a_offset); 

int readc_ifile(file_desc_t *fd);
int writec_ifile(file_desc_t *fd, char c);
int read_ifile(file_desc_t *fd, void *buf, unsigned int nbyte);
int write_ifile(file_desc_t *fd, const void *buf, unsigned int nbyte);

/*------------------------------
  Internal fucntions
  ------------------------------------------------------------*/

/* the inode is already loaded, shortcut the begining of open_ifile() */
int iopen_ifile(file_desc_t *fd, unsigned int inumber, struct inode_s *inode);


#endif
