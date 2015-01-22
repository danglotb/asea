export OBJECTS_SCHED=$(addsuffix .o,\
	 hw manage_ctx)

all: hw manage_ctx 
hw:
	$(CC) -c -g -m32 hw.c
manage_ctx: hw
	$(CC) -c -g -m32 manage_ctx.c
clean: 
	rm -f *.o *~
