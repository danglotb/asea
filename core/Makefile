#ROOT MAKE FILE

# Calling makefiles in File_sys & scheduler

CC	=  gcc
#CFLAGS	=  -Wall 
CFLAGS  +=-ansi -pedantic -m32 
CFLAGS  += -pthread
CFLAGS  += -std=iso9899:1999
CFLAGS  += -Werror 
CFLAGS  += -g

ROOTDIR=.
#ROOTDIR=/home/enseign/ASE/core
BINDIR=./bin

LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

BINARIES= main main_2
OBJECTS	= $(addsuffix .o,\
	   manage_ctx)

all: $(BINARIES) $(OBJECTS)

###------------------------------
### Binaries
###------------------------------------------------------------


main: main.o $(OBJECTS)
main_2: main_2.o $(OBJECTS)

% : %.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ -L$(LIBDIR) $(LIBS)

manage_ctx.o: manage_ctx.h manage_ctx.c
	gcc -I$(INCDIR) -g -m32 -c manage_ctx.c 

%.o: %.c
	$(CC) $(CFLAGS) -c $<

.PHONY: clean realclean depend 
clean:
	$(RM) *.o $(BINARIES) $(OBJECTS)
clean_disk:
	$(RM) *.bin
realclean: clean clean_disk
	$(RM) $(BINDIR)/* *~	
depend : 
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 
