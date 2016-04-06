CC=cilk++
CFLAGS=-O3

all: PARMERGESORTSM PARMERGESORTPM

PARMERGESORTSM:
	$(CC) $(CFLAGS) -o PMSSM ParMergeSortSM.cilk -lcilkutil

PARMERGESORTPM:
	$(CC) $(CFLAGS) -o PMSPM ParMergeSortPM.cilk -lcilkutil

clean:
	rm -rf PMSSM*  core* Test* a_* *.csv *.plt
