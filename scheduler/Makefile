export OBJECTS_SCHED=$(addsuffix .o,\
	 manage_ctx)

all: manage_ctx 

manage_ctx: hw
	$(CC) -c -g -m32 manage_ctx.c
clean: 
	rm -f *.o *~
