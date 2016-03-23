#include<stdio.h>
#include<iostream>
#include<limits.h>
#include<algorithm>
#include"omp.h"
using namespace std;
int M = 4;
void iterFWA(int **graph, int size, int ka, int ia, int ja)
{
        int i, j, k;

        for(k=ka; k<ka+size; k++) {

                for (i=ia; i<ia+size; i++) {

                        for(j=ja; j<ja+size; j++) {
                                graph[i][j] = min(graph[i][j], graph[i][k] + graph[k][j]);
                        }
                }
        }
}
void FWA(int **graph, int V, int k, int i, int j, int size)
{
        int new_size = size/2;

        if(size == M) {
                iterFWA(graph, size, k, i, j);
                return;
        }

        FWA(graph, V, k, i, j, new_size);

        #pragma omp task firstprivate(graph, V, k, i, j, size)
                FWA(graph, V, k, i, j+new_size, new_size);

        #pragma omp task firstprivate(graph, V, k, i, j, size)
                FWA(graph, V, k, i+new_size, j, new_size);
        
        #pragma omp taskwait

        FWA(graph, V, k, i+new_size, j+new_size, new_size);

        FWA(graph, V, k+new_size, i+new_size, j+new_size, new_size);


        #pragma omp task firstprivate(graph, V, k, i, j, size)
                FWA(graph, V, k+new_size, i+new_size, j, new_size);

        #pragma omp task firstprivate(graph, V, k, i, j, size)
                FWA(graph, V, k+new_size, i, j+new_size, new_size);

        #pragma omp taskwait

        FWA(graph, V, k+new_size, i, j, new_size);

}

int main(int argc, char *argv[])
{
        int V = 4;
        if (argc > 1) {
                V = atoi(argv[1]);

                if(V <= 0) {
                        cerr<<"Invalid Argument for V!"<<endl;
                }

                if(argc > 2) {
                        M = atoi(argv[2]);
                        if(M <= 0 || M >= V) {
                                cerr<<"Invalid Argument for M!"<<endl;
                        }
                }

        }

        cout<<"V = "<<V<<endl;
        cout<<"M = "<<M<<endl;

        int **graph = new int*[V];

        for(int row=0; row<V; row++) {
                graph[row] = new int[V];
        }

        for(int i=0; i<V; i++) {
                for(int j=0; j<V; j++) {
                        if(i == j)
                                graph[i][j] = 0;
                        else
                                graph[i][j] = ((i + j) % 3 == 0) ? (i+j)/2 : (i+j);
                }
        }

        cout<<"Original Adjacency Matrix: "<<endl;
        //printAPSP(graph, V);

        double start_time, end_time;
        start_time = omp_get_wtime();

        #pragma omp parallel
        {
                #pragma omp single
                {
                        FWA(graph, V, 0, 0, 0, V);
                }
        }
        end_time = omp_get_wtime();
        cout<<"Total time taken: "<<(end_time - start_time)<<" milliseconds"<<endl;

}
