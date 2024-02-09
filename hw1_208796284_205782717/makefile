# This is a comment line
CC=g++
# CFLAGS will be the options passed to the compiler.
CFLAGS=-c -Wall
OBJECTS = hw1shell.o

all: hw1shell

hw1shell: $(OBJECTS)
	$(CC) $(OBJECTS) -o hw1shell

%.o: %.cpp
	$(CC) $(CFLAGS) $<

run: hw1shell
	export PATH=$$PATH:/usr/bin && ./hw1shell

clean:
	rm -rf *.o hw1shell
