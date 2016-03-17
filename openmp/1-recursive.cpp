#include<stdio.h>
#include<iostream>
#include<limits.h>
#include<algorithm>
#include "omp.h"
using namespace std;

/* maxVertices represents maximum number of vertices that can be present in the graph. */
#ifndef maxVertices
#define maxVertices   64
#endif
#define INF           INT_MAX-1

#define NUM_THREADS 16

/* Input Format: Graph is directed and weighted. First two integers must be number of vertices and edges 
   which must be followed by pairs of vertices which has an edge between them. 
 */
//int min(int a,int b){return (a<b)?a:b;}

int dist[maxVertices][maxVertices];
omp_lock_t dist_locks[maxVertices][maxVertices];
int vertices;
void init(int n)
{
  
        #pragma omp parallel for
        for(int i=0;i<n;i++)
        {
                #pragma omp parallel for
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

void A_loop_FW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n)
{       
	for(int via = Uj; via < Uj + n; via++)
	{
	#pragma omp parallel for
    for(int from = Xi; from < Xi + n; from++)
    {
        #pragma omp parallel for
        for(int to = Xj; to < Xj + n ; to++)
        {
            if(from!=to && from!=via && to!=via)
			{	
				omp_set_lock(&dist_locks[from][to]);
				dist[from][to] = min(dist[from][to],dist[from][via]+dist[via][to]);
				omp_unset_lock(&dist_locks[from][to]);
			}
                        
        }
     }
   }
}


void AFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		A_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		AFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);

		#pragma omp parallel
		{
			AFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
			AFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
		}
		#pragma omp barrier
		
		AFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
		

		AFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		
		#pragma omp parallel
		{
			AFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
			AFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		}
		#pragma omp barrier

		AFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
	}
}

int edges;

int main(int argc, char *argv[])
{      
	//omp_set_num_threads(NUM_THREADS);
	vertices = maxVertices;
	/*initialize dist between all pairs as infinity*/
	init(vertices);
	/* vertices represent number of vertices and edges represent number of edges in the graph. */

	#pragma omp parallel for
	for(int i = 0 ; i < vertices ; i++ )
	{
		#pragma omp parallel for       
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

	AFW(0, 0, 0, 0, 0, 0, vertices);

	for(int i = 0 ; i < vertices; i++ ) 
	{
		cout << "\n";
		for(int j = 0 ; j< vertices ;j++ )
			cout << dist[i][j] << " " ;
	}

	return 0;
}