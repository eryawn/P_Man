.phony all:
all: main

List:
	gcc -Wall ./List.c -c -o List.o

pman: List ./List.h
	gcc -Wall ./PMan.c -std=gnu90 -c -o PMan.o -g

main: List pman
	gcc -Wall ./PMan.o ./List.o -lreadline -o main.exe
	./main.exe

.PHONY clean:
clean:
	-rm -rf *.o *.exe
