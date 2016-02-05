CC=gcc

run: lab7
	./lab7
	
lab7: main.o mem_debug.o
	$(CC) main.o mem_debug.o -o lab7
	rm -f *.o

main.o:
	$(CC) -c main.c

mem_debug.o:
	$(CC) -c mem_debug.c
	
clean:
	rm -f lab7 *.o