rm Test.*

cilk++ -m64 -fPIC -shared -o libspm.so ParMergePM.cilk
mpicxx mpi.cpp  -L. -L$CILKHOME/lib64 -Wl, -lspm -lcilk_main -lcilkrts -lcilkutil
