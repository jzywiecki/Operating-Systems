all: static shared

static:
	@gcc -Wall -c library.c
	@ar rcs library.a library.o

shared:
	@gcc -Wall -fPIC -c library.c
	@gcc -Wall -fPIC -shared library.o -o library.so

clean:
	@rm -f *.o *.a *.so

#@ - don't echo on the output
#fPIC - Position-Independent Code
