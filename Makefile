all: druid

druid: src/game.c src/druid.c src/autoplay.c src/test.c
	gcc -Wall -ansi -o druid src/game.c src/druid.c
	gcc -Wall -ansi -o autoplay src/game.c src/autoplay.c
	gcc -Wall -ansi -o test src/game.c src/test.c

test_suite:
	gcc -Wall -ansi -o test src/game.c src/test.c

test: all
	./test

autoplay: all
	./autoplay

clean:
	rm -f druid test autoplay
