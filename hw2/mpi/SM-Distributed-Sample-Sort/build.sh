cilk++ -m64 -fPIC -shared -o libncr.so ParMergeSM.cilk
mpicxx mpi.cpp  -L. -L$CILKHOME/lib64 -Wl, -rpath=. -lncr -lcilk_main -lcilkrts -lcilkutil
