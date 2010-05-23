all:
	gcc -Wall -ansi -o druid druid.c

clean:
	rm -f druid
