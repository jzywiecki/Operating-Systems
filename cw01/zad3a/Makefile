CFLAGS = -Wall

all: compile_static compile_shared compile_dynamic

static:
	echo "Static"
	@gcc $(CFLAGS) -c library.c
	@ar rcs library.a library.o

shared:
	@echo "Shared"
	@gcc $(CFLAGS) -fPIC -c library.c -o library.o
	@gcc $(CFLAGS) -fPIC -shared library.o -o liblibrary.so

dynamic:
	@echo "Dynamic"
	@gcc $(CFLAGS) -fPIC -c library.c -o library.o
	@gcc $(CFLAGS) -fPIC -shared library.o -o liblibrary.so


clean:
	@echo "Removing everything but the source files!"
	@rm -f *.o *.a *.so

compile_static:
	@make static
	@gcc main.c library.c -o main

compile_shared:
	@make shared
	@gcc -c main.c
	@gcc -o main -L./ main.o -llibrary

compile_dynamic:
	@make dynamic
	@gcc main.c -o main -ldl -D DYNAMIC


test_static:
	@make compile_static
	@./main < ./tests/test.txt > ./results/results_static.txt

test_shared:
	@make compile_shared
	@./main < ./tests/test.txt > ./results/results_shared.txt

test_dynamic:
	@make compile_dynamic
	@./main < ./tests/test.txt > ./results/results_dynamic.txt


#@ - don't echo on the output
#fPIC - Position-Independent Code
