test : testQueFun.o 
	gcc -o edit testQueFun.o -lrt -pthread

testQueFun.o : testQueFun.c
	gcc -c -Wall testQueFun.c

clean :
	rm testQueFun.o edit