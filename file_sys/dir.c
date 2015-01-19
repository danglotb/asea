#include "dir.h"

unsigned int new_entry(file_desc_t* dir_fd) {
    struct entry_s entry;
    int cursor;

    do {
        
        if(dir_fd->fds_pos >= dir_fd->fds_size) {
            break;
        }

        if(read_ifile(dir_fd, &entry, sizeof(struct entry_s)) <= 0) {
            break;
        }

        if(entry.inumber == 0) {
            return cursor;
        }

        cursor++;
    } while(1);

    entry.inumber = 0;
    memset(entry.name, 0, MAX_FILE_LENGTH);

    
    write_ifile(dir_fd, &entry, sizeof(struct entry_s));

    return cursor;
}

int find_entry(file_desc_t* dir_fd, const char* basename) {
    struct entry_s entry;
    int i = 0;

    while(read_ifile(dir_fd, &entry, sizeof(struct entry_s)) > 0) {
        if(strcmp(basename, entry.name) == 0) {
            return i;
        }
        i++;
    }
    return -1;
}

int add_entry(int inumber, const char* name, enum file_type_e type) {
    file_desc_t fd;

    if(strcmp(name, "") == 0) {
        return -1;
    }

    

    if(open_ifile(&fd, inumber) != RETURN_FAILURE) {
        struct entry_s entry;

        if(find_entry(&fd, name) != -1) {
            return -1;
        }

        unsigned int index = new_entry(&fd);

        seek2_ifile(&fd, index*sizeof(struct entry_s));

        entry.inumber = create_ifile(type);

        strncpy(entry.name, name, MAX_FILE_LENGTH);

        write_ifile(&fd, &entry, sizeof(struct entry_s));

        close_ifile(&fd);

        //add .. & .
        if(type == FILE_DIRECTORY) {
            file_desc_t fd_child;
            struct entry_s entry_child;

            if(open_ifile(&fd_child, entry.inumber) != RETURN_FAILURE) {
                entry_child.inumber = entry.inumber;
                strcpy(entry_child.name, ".");
                write_ifile(&fd_child, &entry_child, sizeof(struct entry_s));

                entry_child.inumber = inumber;
                strcpy(entry_child.name, "..");
                write_ifile(&fd_child, &entry_child, sizeof(struct entry_s));

                close_ifile(&fd_child);
            }
        }

        return entry.inumber;
        
    }
    else
        return -1;
    
}

int del_entry(int idir, const char* name) {
    int ret;    
    struct entry_s entry;
    file_desc_t fd;

    open_ifile(&fd, idir);

    ret = find_entry(&fd, name);

    if(ret < 0) {
        return -1;
    }

    seek2_ifile(&fd, ret*sizeof(struct entry_s));

    if(read_ifile(&fd, &entry, sizeof(struct entry_s)) <= 0) {
        return -1;
    }

    delete_ifile(entry.inumber);

    entry.inumber = 0;

    seek2_ifile(&fd, ret*sizeof(struct entry_s));

    if(write_ifile(&fd, &entry, sizeof(struct entry_s)) <= 0) {
        return -1;
    }

    close_ifile(&fd);

    return 0;
}



unsigned int inumber_of_basename(unsigned int idir, const char* basename) {
    file_desc_t fd;
    int ret;
    unsigned int inumber;
    struct entry_s entry;

    open_ifile(&fd, idir);

    ret = find_entry(&fd, basename);

    if(ret < 0) {
        return 0;
    }

    seek2_ifile(&fd, ret*sizeof(struct entry_s));

    if(read_ifile(&fd, &entry, sizeof(struct entry_s)) <= 0) {
        return 0;
    }

    inumber = entry.inumber;

    close_ifile(&fd);

    return inumber;
}

unsigned int inumber_of_path(const char* pathname){
    unsigned int str_cursor;
    unsigned int begin_cursor;
    unsigned int current_inumber;

    if(pathname[0] != '/')
        return 0;

    str_cursor = 1;
    begin_cursor = 1;
    current_inumber = current_super_bloc.sb_inode_root;

    if(pathname[1] == '\0')
        return current_inumber;

    
    do {

        if(pathname[str_cursor] == '/' || pathname[str_cursor] == '\0') {
            char* name;

            if(str_cursor == begin_cursor) {
                return 0;
            }

            name = malloc((str_cursor-begin_cursor+1)*sizeof(char));

            memcpy(name, pathname+begin_cursor, (str_cursor-begin_cursor)*sizeof(char));

            name[str_cursor-begin_cursor] = '\0';

            current_inumber = inumber_of_basename(current_inumber, name);

            if(current_inumber == 0) {
                return 0;
            }
            
            free(name);

            begin_cursor = str_cursor+1;
        }

    } while(pathname[str_cursor++] != '\0');


    return current_inumber;
}