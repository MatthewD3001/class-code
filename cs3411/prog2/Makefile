
all: ctar.o utar.o
	gcc ctar.o -o ctar
	gcc utar.o -o utar

ctar: ctar.o
	gcc ctar.o -o ctar

utar: utar.o
	gcc utar.o -o utar

ctar.o: ctar.c
	gcc -Wall -pedantic -c ctar.c

utar.o: utar.c
	gcc -Wall -pedantic -c utar.c

clean:
	rm *.o utar ctar
