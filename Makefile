all: tp
tp: assembler.o main.o
	gcc -o tp assembler.o main.o
assembler.o: assembler.c assembler.h
	gcc -o assembler.o -c assembler.c
main.o: main.c assembler.h
	gcc -o main.o -c main.c

clean:
	rm -rf *.o
