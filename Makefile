#Rob Casale
#CS416 Assignment 5
#Section 4

COMPILER = gcc
CCFLAGS = -Wall -g

all: testshell

testshell: testshell.o
	$(COMPILER) $(CCFLAGS) testshell.o -o testshell

testshell.o: testshell.c
	$(COMPILER) $(CCFLAGS) -c testshell.c
clean:
	rm -rf *.o testshell *.a *.gch
