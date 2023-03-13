default: all

all: compile_static

compile_static:
	@gcc -Wall -c timers.c
	@gcc main.c timers.c -o main

test:
	@gcc -Wall -c timers.c
	@gcc main.c timers.c -o main
	@./main ./

clean:
	@rm main