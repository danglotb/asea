#ROOT MAKE FILE

# Calling makefiles in File_sys & scheduler

CC	=  gcc
CFLAGS	=  -Wall -ansi -pedantic -m32 -lpthread
CFLAGS  += -Werror -std=iso9899:1999
CFLAGS  += -g

ROOTDIR=.
BINDIR=./bin

LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

BINARIES= main


all: $(BINARIES)

###------------------------------
### Binaries
###------------------------------------------------------------
main: main.o

% : %.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ -L$(LIBDIR) $(LIBS)

.PHONY: clean realclean depend 
clean:
	$(RM) *.o $(BINARIES)
clean_disk:
	$(RM) *.bin
realclean: clean clean_disk
	$(RM) $(BINDIR)/* *~	
depend : 
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 
