#ifndef _DIR_H
#define _DIR_H

#include "ifile.h"
#include "mount.h"

#define MAX_FILE_LENGTH 64

struct entry_s {
  char name[MAX_FILE_LENGTH];
  unsigned int inumber;
};

unsigned int new_entry(file_desc_t* dir_fd);

int find_entry(file_desc_t* dir_df, const char* basename);

unsigned int inumber_of_basename(unsigned int idir, const char* basename);

unsigned int inumber_of_path(const char* pathname);

int add_entry(int inumber, const char* name, enum file_type_e type);

int del_entry(int idir, const char* name);

#endif