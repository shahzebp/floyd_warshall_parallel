CC=icc
CFLAGS=-O3

all: FW FWR1 FWR2 FWR3

FW: 
	$(CC) $(CFLAGS) -o fw FW.cpp

FWR1: 
	$(CC) $(CFLAGS) -o fwr1 FWRecursive1.cpp

FWR2: 
	$(CC) $(CFLAGS) -o fwr2 FWRecursive2.cpp

FWR3: 
	$(CC) $(CFLAGS) -o fwr3 FWRecursive3.cpp

clean:
	rm -rf fw*  core* Test* a_*
