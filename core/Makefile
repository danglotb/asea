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
OBJECTS	= $(addsuffix .o,\
	   manage_ctx)

all: $(BINARIES) $(OBJECTS)

###------------------------------
### Binaries
###------------------------------------------------------------


main: main.o $(OBJECTS)

% : %.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@ $^ -L$(LIBDIR) $(LIBS)

manage_ctx.o:
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
