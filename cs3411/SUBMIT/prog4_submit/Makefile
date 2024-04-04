all: prose.o step6.o step5.o step4.o step3.o step2.o step1.o
	gcc prose.o -o prose
	gcc step6.o -o step6
	gcc step5.o -o step5
	gcc step4.o -o step4
	gcc step3.o -o step3
	gcc step2.o -o step2
	gcc step1.o -o step1

prose: prose.o
	gcc prose.o -o prose

prose.o: prose.c
	gcc -Wall -c prose.c

step6: step6.o
	gcc step6.o -o step6

step6.o: step6.c
	gcc -Wall -c step6.c

step5: step5.o
	gcc step5.o -o step5

step5.o: step5.c
	gcc -Wall -c step5.c

step4: step4.o
	gcc step4.o -o step4

step4.o: step4.c
	gcc -Wall -c step4.c

step3: step3.o
	gcc step3.o -o step3

step3.o: step3.c
	gcc -Wall -c step3.c

step2: step2.o
	gcc step2.o -o step2

step2.o: step2.c
	gcc -Wall -c step2.c

step1: step1.o
	gcc step1.o -o step1

step1.o: step1.c
	gcc -Wall -c step1.c

clean:
	rm prose step6 step5 step4 step3 step2 step1 *.o
