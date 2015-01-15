

ROOTDIR=/home/enseign/ASE
#ROOTDIR=.

CC	=  gcc
CFLAGS	=  -Wall -Werror -ansi -pedantic
CFLAGS  += -std=iso9899:1999
CFLAGS  += -g
LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= vm mknfs dfs ndir pdir nfile pfile dfile cfile ddir
OBJECTS	= $(addsuffix .o,\
	  tools ifile mount mbr bloc_allocator inode drive dir)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Binaries
###------------------------------------------------------------
vm: vm.o $(OBJECTS)
mknfs: mknfs.o $(OBJECTS)
dfs: dfs.o $(OBJECTS)
ndir: ndir.o $(OBJECTS)
pdir: pdir.o $(OBJECTS)
nfile: nfile.o $(OBJECTS)
pfile: pfile.o $(OBJECTS)
dfile: dfile.o $(OBJECTS)
cfile: cfile.o $(OBJECTS)
ddir: ddir.o $(OBJECTS)
% : %.o
	$(CC) -o $@ $^ -L$(LIBDIR) $(LIBS)

###------------------------------
### #include dependences 
###------------------------------------------------------------
# you may fill these lines with "make depend"
drive.o: drive.c drive.h
mbr.o: mbr.c mbr.h
bloc_allocator.o: bloc_allocator.c bloc_allocator.h
inode.o: inode.c inode.h
cfile.o: cfile.c ifile.h inode.h mount.h
dfile.o: dfile.c ifile.h inode.h mount.h
nfile.o: nfile.c ifile.h inode.h mount.h
pfile.o: pfile.c ifile.h inode.h mount.h
ifile.o: ifile.c inode.h ifile.h 
mount.o: mount.c config.h  
tools.o: tools.c 
dir.o: dir.c

%.o: %.c
	$(CC) $(CFLAGS) -c $< 

###------------------------------
### Misc.
###------------------------------------------------------------
.PHONY: clean realclean depend
clean:
	$(RM) *.o $(BINARIES)
realclean: clean 
depend : 
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 

