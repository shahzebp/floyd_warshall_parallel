#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<limits.h>
#include<algorithm>
#include<omp.h>
using namespace std;

/* maxVertices represents maximum number of vertices that can be present in the graph. */
#ifndef maxVertices
#define maxVertices   8192
#endif
#define INF           INT_MAX-1
#define OMP_NUM_THREADS    16

int vertices;
int m;
int dist[maxVertices][maxVertices];

void print_matrix()
{
    for(int i = 0 ; i < vertices; i++ ) 
    {
        cout << "\n";
        for(int j = 0 ; j< vertices ;j++ )
            cout << dist[i][j] << " " ;
    }
}

void init(int n)
{
    for(int i=0;i<n;i++)
    {
        for(int j=0;j<n;j++)
        {
            if(i==j)
            {
                dist[i][j] = 0;
            }
            else
            {
                dist[i][j] = INF;
            }
        }
    }
}

void floyd_warshall_iter(int Xi, int Xj,
                                        int Ui, int Uj,
                                        int Vi, int Vj,
                                        int size)
{
    for(int k = Vi; k < (Vi + size);k++)
    {
        for(int i = Xi; i < (Xi + size); i++)
        {
            for(int j = Xj; j < (Xj + size); j++)
            {
                if(i != j && j != k && i != k) 
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }
}

void DFW(int Xi, int Xj,
                int Ui, int Uj,
                int Vi, int Vj,
                int n)
{
    if (n == m)
        floyd_warshall_iter(Xi,Xj,
                 Ui,Uj,
                 Vi, Vj,n);
    else
    {
            #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
            {
                        DFW(Xi,Xj, Ui,Uj, Vi,Vj, n/2);
                    }
              #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
              {
                        DFW(Xi,Xj + n/2, Ui,Uj, Vi,Vj + n/2, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)                 
            {
                        DFW(Xi + n/2,Xj, Ui + n/2,Uj, Vi,Vj, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {                       
                        DFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj, Vi,Vj + n/2, n/2);
                    }
                #pragma omp taskwait

             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {        
                        DFW(Xi,Xj, Ui,Uj + n/2,  Vi + n/2,Vj, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {                    
                        DFW(Xi,Xj + n/2, Ui,Uj + n/2, Vi + n/2,Vj + n/2, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
             {                    
                        DFW(Xi + n/2,Xj, Ui + n/2,Uj + n/2, Vi + n/2,Vj, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)         
            {            
                        DFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj + n/2, Vi + n/2,Vj + n/2, n/2);
                    }
        #pragma omp taskwait
    }
}

void CFW(int Xi, int Xj,
                int Ui, int Uj,
                int Vi, int Vj,
                int n)
{
    if (n == m)
        floyd_warshall_iter(Xi,Xj,
                 Ui,Uj,
                 Vi, Vj,n);
    else
    {
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {            
                        CFW(Xi,Xj, Ui,Uj, Vi,Vj, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {                    
                        CFW(Xi + n/2,Xj, Ui + n/2,Uj, Vi,Vj, n/2);
                    }
                #pragma omp taskwait
        
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
            {            
                        DFW(Xi,Xj + n/2, Ui,Uj, Vi,Vj + n/2, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {                                      
                        DFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj, Vi,Vj + n/2, n/2);
            }                        
                #pragma omp taskwait

             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {
                        CFW(Xi,Xj + n/2, Ui,Uj + n/2,  Vi + n/2,Vj + n/2, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {                    
                        CFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj + n/2, Vi + n/2,Vj + n/2, n/2);
                    }
                #pragma omp taskwait

             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {        
                        DFW(Xi,Xj, Ui,Uj + n/2, Vi + n/2,Vj, n/2);
                    }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)
             {             
                        DFW(Xi + n/2,Xj, Ui + n/2,Uj + n/2, Vi,Vj + n/2, n/2);
                    }
        #pragma omp taskwait
    }
}

void BFW(int Xi, int Xj,
                int Ui, int Uj,
                int Vi, int Vj,
                int n)
{
    if (n == m)
        floyd_warshall_iter(Xi,Xj,
                 Ui,Uj,
                 Vi, Vj,n);
    else
    {
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
            {            
                BFW(Xi,Xj, Ui,Uj, Vi,Vj, n/2);
            }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
             {        
                BFW(Xi,Xj + n/2, Ui,Uj, Vi,Vj + n/2, n/2);            
            }
        #pragma omp taskwait
                                
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)         
            {
                DFW(Xi + n/2,Xj, Ui + n/2,Uj, Vi,Vj, n/2);    
                }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {            
                DFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj, Vi,Vj + n/2, n/2);    
            }
        #pragma omp taskwait

             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {
                BFW(Xi + n/2,Xj, Ui + n/2,Uj + n/2, Vi + n/2,Vj, n/2);
            }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n) 
            {            
                BFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj + n/2, Vi + n/2,Vj + n/2, n/2);
            }
        #pragma omp taskwait

             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {            
                DFW(Xi,Xj, Ui,Uj + n/2, Vi + n/2,Vj, n/2);
            }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)
            {            
                DFW(Xi,Xj + n/2, Ui,Uj + n/2,  Vi + n/2,Vj + n/2, n/2);
            }
        #pragma omp taskwait
    }
}

void AFW(int Xi, int Xj,
            int Ui, int Uj,
                                        int Vi, int Vj,
                                        int n)
{
    if (n == m)
        floyd_warshall_iter(Xi,Xj,
                                         Ui,Uj,
                                         Vi, Vj,n);
    else
    {
        AFW(Xi,Xj, Ui,Uj, Vi, Vj, n/2);
        
            #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)
            { 
                BFW(Xi,Xj + n/2, Ui,Uj, Vi, Vj + n/2, n/2);                        
            }
            #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)
            { 
                CFW(Xi + n/2,Xj, Ui + n/2,Uj, Vi, Vj, n/2);
            }
        #pragma omp taskwait                                
        
        DFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj, Vi, Vj + n/2, n/2);        
        AFW(Xi + n/2,Xj + n/2, Ui + n/2,Uj + n/2, Vi + n/2, Vj + n/2, n/2);

             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {            
                BFW(Xi + n/2,Xj, Ui + n/2,Uj + n/2, Vi + n/2, Vj, n/2);
            }
             #pragma omp task firstprivate(Xi,Xj, Ui,Uj, Vi,Vj,n)     
            {            
                CFW(Xi,Xj + n/2, Ui,Uj + n/2,  Vi + n/2, Vj + n/2, n/2);
            }
        #pragma omp taskwait
        DFW(Xi,Xj, Ui,Uj + n/2, Vi + n/2, Vj, n/2);
    }
}


int main(int argc, char *argv[])
{
    char *arg_vertices = getenv("N_VERTICES");
    char *arg_m_val    = getenv("M_VAL");
    vertices=atoi(arg_vertices);
    m = atoi(arg_m_val);
    init(vertices);

       for(int i = 0 ; i < vertices ; i++ )
        {
                for(int j = 0 ; j< vertices; j++ )
                {
                        if( i == j )
                                dist[i][j] = 0;
                        else {
                                int num = i + j;

                                if (num % 3 == 0)
                                         dist[i][j] = num / 2;
                                else if (num % 2 == 0)
                                         dist[i][j] = num * 2;
                                else
                                         dist[i][j] = num;
                        }
                }
        }

    omp_set_dynamic(0);
    omp_set_num_threads(omp_get_num_procs());
    double start_time = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        {
                AFW(0,0,0,0,0,0,vertices);
        }
    }
   double end_time = omp_get_wtime();
   cout <<endl<<endl<< end_time-start_time <<endl<<endl;
    return 0;
}
