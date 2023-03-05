CFLAGS = -Wall -std=gnu11

all: compile_static compile_shared compile_dynamic

static:
	echo "Static"
	@gcc $(CFLAGS) -c library.c
	@ar rcs library.a library.o

shared:
	@echo "Shared"
	@gcc $(CFLAGS) -fPIC -c library.c -o library.o
	@gcc $(CFLAGS) -fPIC -shared library.o -o library.so

dynamic:



clean:
	echo "Removing everything but the source files!"
	@rm -f *.o *.a *.so

compile_static: static
	@make static
	@gcc main.c library.c -o main

compile_shared: shared
	@make shared
	@gcc -c main.c
	@gcc -o main -L./ main.o -llibrary

compile_dynamic: dynamic




#@ - don't echo on the output
#fPIC - Position-Independent Code
