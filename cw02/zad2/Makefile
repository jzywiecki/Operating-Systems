default: all

all: compile_static

compile_static:
	@gcc -Wall -c timers.c
	@gcc main.c timers.c -o main

test: #to test separately functions you have to comment code in main.c
	@gcc -Wall -c timers.c
	@gcc main.c timers.c -o main
	@./main file_from.txt file_to.txt

clean:
	@echo "Removing everything but the source files!"
	@rm -f *.o *.a *.so main