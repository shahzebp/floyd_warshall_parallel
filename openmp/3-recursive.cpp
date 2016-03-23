#include<stdio.h>
#include<iostream>
#include <cstdlib>
#include<limits.h>
#include<algorithm>
#include"omp.h"
using namespace std;

/* maxVertices represents maximum number of vertices that can be present in the graph. */
#ifndef maxVertices
#define maxVertices   64
#endif
#define INF           INT_MAX-1


/* Input Format: Graph is directed and weighted. First two integers must be number of vertices and edges 
   which must be followed by pairs of vertices which has an edge between them. 
 */
//int min(int a,int b){return (a<b)?a:b;}

int dist[maxVertices][maxVertices];
omp_lock_t dist_locks[maxVertices][maxVertices];
int vertices;

void init(int n)
{
   #pragma omp for nowait
   for(int i=0;i<n;i++)
   {
                #pragma omp for nowait
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

void F_loop_FW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n)
{   
	#pragma omp parallel
	{	
			int nthreads = omp_get_num_threads();
	    	int id = omp_get_thread_num();  
		for(int via = Uj+id; via < Uj + n; via+=nthreads)
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
}

void DFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		#pragma omp parallel sections
		{
			#pragma omp section 
			{
				DFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);
			}
			#pragma omp section 
			{
				DFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
			}
			#pragma omp section 
			{
				DFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
			}
			#pragma omp section 
			{
				DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
			}
		}
		#pragma omp barrier

		#pragma omp parallel sections
		{		
				#pragma omp section 
				{
					DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
				}
				#pragma omp section 
				{
					DFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
				}
				#pragma omp section 
				{
					DFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
				}
				#pragma omp section 
				{
					DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
				}
		}
		#pragma omp barrier
	}
}

void BFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {

		#pragma omp parallel sections
		{
			#pragma omp section
			{
				BFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);
			}
			#pragma omp section
			{
				BFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
			}
		}
		#pragma omp barrier


		#pragma omp parallel sections
		{		
				#pragma omp section
				{
					DFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
				}
				#pragma omp section
				{
					DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
				}
		}
		#pragma omp barrier


		#pragma omp parallel sections
		{
				#pragma omp section
				{
					BFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
				}
				#pragma omp section
				{
					BFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
				}
		}
		#pragma omp barrier


		#pragma omp parallel sections
		{	
				#pragma omp section
				{
					DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
				}
				#pragma omp section
				{
					DFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
				}
		}
		#pragma omp barrier

	}

}

void CFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);
	else {
		#pragma omp parallel sections
		{
			#pragma omp section
			{
				CFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);
			}
			#pragma omp section
			{
				CFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
			}
		}
		#pragma omp barrier


		#pragma omp parallel sections
		{
			#pragma omp section
			{
				DFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
			}
			#pragma omp section
			{
				DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
			}
		}
		#pragma omp barrier


		#pragma omp parallel sections
		{
			#pragma omp section
			{
				CFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
			}
			#pragma omp section
			{
				CFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
			}
		}
		#pragma omp barrier


		#pragma omp parallel sections
		{
			#pragma omp section
			{
				DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
			}
			#pragma omp section
			{
				DFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi, Vj + n/2, n/2);
			}
		}
		#pragma omp barrier

	}

}

void AFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		AFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);

		#pragma omp parallel sections 
		{
				#pragma omp section
				{
					BFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
				}
				#pragma omp section
				{
					CFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
				}
		}
		#pragma omp barrier

		
		DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
		

		AFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		
		#pragma omp parallel sections
		{
				#pragma omp section
				{
					BFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
				}
				#pragma omp section
				{
					CFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
				}
		}
		#pragma omp barrier


		DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
	}
}

int edges;

int main(int argc, char *argv[])
{      
	char *arg_vertices = getenv("N_VERTICES");
     vertices = atoi(arg_vertices);

	init(vertices);

	#pragma omp for nowait
	for(int i = 0 ; i < vertices ; i++ )
	{
		#pragma omp for nowait      
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
	#pragma omp barrier

	double start = omp_get_wtime();
		AFW(0, 0, 0, 0, 0, 0, vertices);
    double end = omp_get_wtime();

	for(int i = 0 ; i < vertices; i++ ) 
	{
		cout << "\n";
		for(int j = 0 ; j< vertices ;j++ )
			cout << dist[i][j] << " " ;
	}

	printf("\nTime elapsed: %.16f\n", end - start);
	return 0;
}
