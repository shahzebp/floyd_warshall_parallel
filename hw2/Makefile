CC=cilk++
CFLAGS=-O3

all: PARMERGESORTSM

PARMERGESORTSM:
	$(CC) $(CFLAGS) -o PMSSM ParMergeSortSM.cilk -lcilkutil

clean:
	rm -rf PMSSM*  core* Test* a_* *.csv *.plt
