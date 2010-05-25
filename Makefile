all: druid

druid: druid.c
	gcc -Wall -ansi -o druid druid.c

test_suite: test.c
	gcc -Wall -ansi -o test test.c

test: test_suite
	./test

clean:
	rm -f druid test
