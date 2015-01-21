#ROOT MAKE FILE

# Calling makefiles in File_sys & scheduler

export CC	=  gcc
export CFLAGS	=  -Wall -ansi -pedantic -m32
export CFLAGS  += -Werror -std=iso9899:1999
export CFLAGS  += -g

export FILE_DIR= ./file_sys
export SCHED_DIR= ./scheduler

all: file sched

file: sched
	@(cd $(FILE_DIR) && $(MAKE))
sched:
	@(cd $(SCHED_DIR) && $(MAKE))

.PHONY: clean realclean depend 
clean: clean_file_sys clean_sched
clean_file_sys:
	$(RM) ./$(FILE_DIR)/*.o $(BINARIES)
clean_sched:
	$(RM) ./$(SCHED_DIR)/*.o $(BINARIES)





