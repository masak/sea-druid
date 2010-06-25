all: druid

druid: src/*.h src/*.c
	gcc -Wall -ansi -o druid src/game.c src/autoplay.c src/druid.c
	gcc -Wall -ansi -o test src/game.c src/test.c

test_suite:
	gcc -Wall -ansi -o test src/game.c src/test.c

test: all
	./test

autoplay: all
	./druid --autoplay

clean:
	rm -f druid test autoplay
