S=main3.c
O=$(addsuffix .o , $(basename $(S)))
E=$(suffix $(S))
CC=/opt/intel/cc64/bin/icc 
F77=/opt/intel/fc64/bin/ifort
CC=g++
LIBS=-L/usr/X11/lib64 -fopenmp -lX11
APP=shark
INCLUDES=

ifeq  ($(E),.f)
LN=$(F77)
else
LN=$(CC)
endif

CFLAGS= -fopenmp
LFLAGS=$(CFLAGS)
OBJS=item.o $(O) 

exec:  program
	./$(APP)

program: $(OBJS)
	$(LN) $(LFLAGS) $(OBJS) $(LIBS) -o $(APP)

item.o: common/x.h common/shark.h 
	$(CC) $(CFLAGS) $(INCLUDES) -c common/item.c

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<
	
.f.o:
	$(F77) $(CFLAGS) $(INCLUDES) -c $<
	
clean:
	rm shark a.out *.o
