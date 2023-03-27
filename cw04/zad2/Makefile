
all: tests

tests:
	@gcc siginfo.c -Wall -o siginfo
	@./siginfo
	@gcc nodefer.c -Wall -o nodefer
	@./nodefer
	@gcc resethand.c -Wall -o resethand
	@./resethand

clean:
	@rm siginfo resethand nodefer



