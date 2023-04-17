
all: main

main:
	@gcc -Wall -c timers.c
	@gcc main.c timers.c -o main
	@gcc integral.c -o integral

clean:
	@rm main integral timers.o