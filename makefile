CC = gcc
CFLAGS = -Wall
DEPS = testsim.c runsim.c
OBJ = testsim.o runsim.o

all: runsim testsim

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
runsim: runsim.o
	$(CC) $(CFLAGS) -o $@ $^
	
testsim: testsim.o
	$(CC) $(CFLAGS) -o $@ $^