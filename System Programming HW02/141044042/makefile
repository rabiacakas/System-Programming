all:	program

program.o:
	gcc -c -ansi -pedantic -errors main.c -Wall

program:	program.o
	gcc -o ./listdir main.o

clean:
	-rm -f main.o
	-rm -f listdir
