edit : clock.o getTime.o
	gcc -o edit clock.o getTime.o -lrt -pthread

clock.o : clock.c clock.h
	gcc -c -Wall clock.c clock.h logOutput.h

getTime.o : getTime.c getTime.h
	gcc -c -Wall getTime.c getTime.h logOutput.h

clean :
	rm clock.o getTime.o edit clock.h.gch getTime.h.gch