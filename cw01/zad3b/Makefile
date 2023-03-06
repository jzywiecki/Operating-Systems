CFLAGS = -Wall

OPT=-O0

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
	@gcc main.c library.c -o main $(opt)

compile_shared:
	@make shared
	@gcc -c main.c
	@gcc -o main -L./ main.o -llibrary $(opt)

compile_dynamic:
	@make dynamic
	@gcc main.c -o main -ldl -D DYNAMIC $(opt)


test_static:
	@read opt; \
	make compile_static OPT=$$opt; \
	./main < ./tests/test.txt > ./results/results_static_$$opt.txt;

test_shared:
	@read opt; \
	make compile_shared OPT=$$opt; \
	./main < ./tests/test.txt > ./results/results_shared_$$opt.txt;

test_dynamic:
	@read opt; \
	make compile_dynamic OPT=$$opt; \
	./main < ./tests/test.txt > ./results/results_dynamic_$$opt.txt;


#@ - don't echo on the output
#fPIC - Position-Independent Code
