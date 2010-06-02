all: druid

druid:
	gcc -Wall -ansi -o druid src/game.c src/druid.c

test_suite:
	gcc -Wall -ansi -o test src/game.c src/test.c

test: test_suite
	./test

clean:
	rm -f druid test
