# $Id: Makefile 78 2007-05-21 17:33:37Z skaczmarek $
##############################################################################
ROOTDIR=/home/enseign/ASE

CC	= gcc
CFLAGS	= -Wall -ansi -pedantic 
CFLAGS  += -g
INCS    = -I$(ROOTDIR)/include
LIBS    = -L$(ROOTDIR)/lib -lhardware

###------------------------------
### Main rules 
###------------------------------------------------------------
tmr :tmr.o
	$(CC) $(CFLAGS) -o tmr tmr.o $(LIBS) 
tmr.o :tmr.c hw_config.h 
	$(CC) $(CFLAGS) $(INCS) -c tmr.c 

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o 
realclean: clean 
	$(RM) tmr
