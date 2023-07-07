#VPATH = ../clock/
vpath %.c ../clock
vpath %.h ../clock	#for makefile

edit : testQueFun.o clock.o getTime.o
	gcc -o edit testQueFun.o  clock.o getTime.o -lrt -pthread

testQueFun.o : testQueFun.c
	gcc -c -Wall testQueFun.c

clock.o : clock.c clock.h
	#add path for gcc
	gcc -c ../clock/clock.c ../clock/clock.h

getTime.o : getTime.c getTime.h
	gcc -c ../clock/getTime.c ../clock/getTime.h

.PHONY : clean
clean :
	-rm -f *.o edit