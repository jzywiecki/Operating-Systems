default: all

all: compile_static

compile_static:
	@gcc -Wall -c timers.c
	@gcc main.c timers.c -o main


clean:
	@rm main