/* ------------------------------
   $Id: file.h 7085 2013-10-18 15:37:14Z marquet $
   ------------------------------------------------------------

   File manipulation. The main library interface.
   Philippe Marquet, november 2002
   
*/

#ifndef _FILE_H_
#define _FILE_H_

#include "tools.h"	/* for RETURN_{FAILURE,SUCCESS} */
#include "inode.h"	/* for file_type_e */
#include "cfile.h"

/* most of the following functions return RETURN_FAILURE in case of
   failure. */

/*------------------------------
  File creation and deletion
  ------------------------------------------------------------*/

/* return RETURN_FAILURE in of failure (pre-existing file, full
   volume...) */ 
int create_file(const char *pathname, enum file_type_e type);

/* return RETURN_FAILURE in of failure (non pre-existing file, non
   empty directory...) */
int delete_file(const char *pathname);

/*------------------------------
  File management
  ------------------------------------------------------------*/

int open_file(file_desc_t *fd, const char *pathname);
void close_file(file_desc_t *fd);
void flush_file(file_desc_t *fd);
void seek_file(file_desc_t *fd, int offset);

/*------------------------------
  File accesses
  ------------------------------------------------------------*/

/* return the conversion to an int of the current char in the file.
   Return READ_EOF if the file is at end-of-file. */
int readc_file(file_desc_t *fd);

/* write a char in the file.
   Fail if there is no place on device. */  
int writec_file(file_desc_t *fd, char c);

/* read nbytes char from the file and copy them in the buffer.
   Return the number of actually read char; READ_EOF if the file is at
   end-of-file. */ 
int read_file(file_desc_t *fd, void *buf, unsigned int nbyte);

/* write nbyte char from the buffer on the file.
   Return the number of char writen, RETURN_FAILURE in case of error. */
int write_file(file_desc_t *fd, const void *buf, unsigned int nbyte);

#endif
