all: encode.o decode.o
	gcc encode.o -o encode
	gcc decode.o -o decode

encode: encode.o
	gcc encode.o -o encode

decode: decode.o
	gcc decode.o -o decode

encode.o: encode.c
	gcc -Wall -pedantic -c encode.c

decode.o: decode.c
	gcc -Wall -pedantic -c decode.c

clean:
	rm *.o encode decode
