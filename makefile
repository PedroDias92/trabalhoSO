CC=gcc
CFLAGS=-Wall

all: notebook

notebook: notebook.c
	gcc -o notebook notebook.c

clean:
	rm notebook *.o

run: notebook
	./notebook
