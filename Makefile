CC=icc
CFLAGS=-O3

all: FW FWR1 FWR2 FWR3

FW: 
	$(CC) $(CFLAGS) -o fw FW.cpp -I $(HOME)/cilk/include/cilktools/

FWR1: 
	$(CC) $(CFLAGS) -o fwr1 FWRecursive1.cpp -I $(HOME)/cilk/include/cilktools/

FWR2: 
	$(CC) $(CFLAGS) -o fwr2 FWRecursive2.cpp -I $(HOME)/cilk/include/cilktools/

FWR3: 
	$(CC) $(CFLAGS) -o fwr3 FWRecursive3.cpp -I $(HOME)/cilk/include/cilktools/

clean:
	rm -rf fw*  core* Test* a_* cilkview*
