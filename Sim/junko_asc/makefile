// CFLAGS = -D_SIMEB_CAP_GROUP 
CFLAGS = -D_SIMEB_CAP_RELATIVITY -D_SIMEB_CAP_ACCEL

INCOPT = -I/usr/include/g++
CC = g++ -c -O3 -ffast-math $(CFLAGS) $(INCOPT)
LINK = g++ -O3 -lm  -ffast-math $(CFLAGS)
OBJS = setup.o vector.o shape.o illumin.o plane.o source.o particle.o column.o relativity.o simeb.o trap.o
HDRS = setup.h vector.h shape.h illumin.h plane.h source.h particle.h column.h relativity.h cap.h trap.h

simeb: $(OBJS) makefile
	$(LINK) -o simeb $(OBJS)

simeb.o: simeb.cc simeb.h makefile
	$(CC) simeb.cc 

cap.o: cap.cc simeb.h makefile
	$(CC) cap.cc 

setup.o: setup.cc simeb.h makefile
	$(CC) setup.cc 

shape.o: shape.cc simeb.h makefile
	$(CC) shape.cc 

illumin.o: illumin.cc simeb.h makefile
	$(CC) illumin.cc 

plane.o: plane.cc simeb.h makefile
	$(CC) plane.cc 

source.o: source.cc simeb.h makefile
	$(CC) source.cc 

particle.o: particle.cc simeb.h makefile
	$(CC) particle.cc 

column.o: column.cc simeb.h makefile
	$(CC) column.cc 

vector.o: vector.cc simeb.h makefile
	$(CC) vector.cc 

relativity.o: relativity.cc simeb.h makefile
	$(CC) relativity.cc 

simeb.h: $(HDRS) makefile
	touch simeb.h

trap.o: trap.cc simeb.h makefile
	$(CC) trap.cc 

clean:
	rm *.o


