
#change ROOTDIR uncomment / comment


#ROOTDIR=/home/enseign/ASE
CC	=  gcc
CFLAGS = -m32
#doesn't work at home
CFLAGS	+=  -ansi -pedantic 
CFLAGS  += -std=iso9899:1999 
CFLAGS  += -Werror -Wall -lpthread
#comment / uncommentm
CFLAGS  += -g
ROOTDIR=/home/enseign/ASE/core
BINDIR=./bin

LIBDIR  =  $(ROOTDIR)/lib
INCDIR  =  $(ROOTDIR)/include
CFLAGS  += -I$(INCDIR)
LIBS    = -lhardware

###------------------------------
### Main targets 
###------------------------------------------------------------
BINARIES= vm mknfs dfs ndir pdir nfile pfile dfile cfile ddir prompt init_fs
OBJECTS	= $(addsuffix .o,\
	  tools ifile mount mbr bloc_allocator inode drive dir manage_ctx)

all: $(BINARIES) $(OBJECTS)


###------------------------------
### Binaries
###------------------------------------------------------------
init_fs: init_fs.o $(OBJECTS)
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
prompt: prompt.o $(OBJECTS)

% : %.o
	$(CC) $(CFLAGS) -o $(BINDIR)/$@  $^ -L$(LIBDIR) $(LIBS)

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
manage_ctx.o: manage_ctx.c manage_ctx.h
	gcc -I$(INCDIR) -g -m32 -c manage_ctx.c 
ifile.o: ifile.c inode.h ifile.h 
mount.o: mount.c config.h manage_ctx.o
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
clean_disk:
	$(RM) *.bin
realclean: clean clean_disk
	$(RM) $(BINDIR)/* *~	
depend : 
	$(CC) $(CFLAGS) -MM $(INCDIR) *.c 

