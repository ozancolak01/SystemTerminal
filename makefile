all: terminal

terminal: terminal.o my_system.o
	gcc terminal.o my_system.o -o terminal

my_system.o: my_system.c my_system.h
	gcc -g -c my_system.c -o my_system.o

terminal.o: terminal.c my_system.h
	gcc -g -c terminal.c -o terminal.o

clean:
	rm -f *.txt terminal.o my_system.o terminal
