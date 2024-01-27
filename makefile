# This is a comment line
CC=g++
# CFLAGS will be the options passed to the compiler.
CFLAGS=-c –Wall
OBJECTS = hm1shell.o
all: prog

prog: $(OBJECTS)
		$(CC) $(OBJECTS) -o hw1shell

%.o: %.cpp
		$(CC) $(CFLAGS) $<

clean:
		rm -rf *.o