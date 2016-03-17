CC=icc
CFLAGS=-O3

all: 
	$(CC) $(CFLAGS) -o fw FWRecursive3.cpp

clean:
	rm -rf fw  core* Test* a_*
