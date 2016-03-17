#include<stdio.h>
#include<iostream>
#include <cstdlib>

#include<limits.h>
#include<algorithm>
#include<cilk/cilk.h>
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
int vertices;
void init(int n)
{
        cilk_for(int i=0;i<n;i++)
        {
                cilk_for(int j=0;j<n;j++)
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
	for(int via = Uj; via < Uj + n; via++)
	{
	cilk_for(int from = Xi; from < Xi + n; from++)
        {
                cilk_for(int to = Xj; to < Xj + n ; to++)
                {
                        if(from!=to && from!=via && to!=via)
			{
				dist[from][to] = min(dist[from][to],dist[from][via]+dist[via][to]);
			}
                        
                }
        }
   }
}

void DFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		DFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);
		DFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
		DFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
		DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);

		DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
		DFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		DFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
		DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
	}
}

void BFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		cilk_spawn BFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);
		BFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
		cilk_sync;

		cilk_spawn DFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
		DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
		cilk_sync;

		cilk_spawn BFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
		BFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		cilk_sync;

		cilk_spawn DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
		DFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		cilk_sync;
	}

}

void CFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);
	else {
		cilk_spawn CFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);
		CFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
		cilk_sync;

		cilk_spawn DFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
		DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
		cilk_sync;

		cilk_spawn CFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		CFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		cilk_sync;

		cilk_spawn DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
		DFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi, Vj + n/2, n/2);
		cilk_sync;
	}

}

void AFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		F_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		AFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);

		cilk_spawn BFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
		CFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
		cilk_sync;
		
		DFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
		

		AFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		
		cilk_spawn BFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
		CFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		cilk_sync;

		DFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
	}
}

int edges;

int main(int argc, char *argv[])
{      
	vertices = maxVertices;
	/*initialize dist between all pairs as infinity*/
	init(vertices);
	/* vertices represent number of vertices and edges represent number of edges in the graph. */

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

	AFW(0, 0, 0, 0, 0, 0, vertices);

	for(int i = 0 ; i < vertices; i++ ) 
	{
		cout << "\n";
		for(int j = 0 ; j< vertices ;j++ )
			cout << dist[i][j] << " " ;
	}

	return 0;
}
