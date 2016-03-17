#include<stdio.h>
#include<iostream>
#include<limits.h>
#include<algorithm>
#include<cilk/cilk.h>
using namespace std;

/* maxVertices represents maximum number of vertices that can be present in the graph. */
#ifndef maxVertices
#define maxVertices   8192
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

void A_loop_FW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n)
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


void AFW(int Xi, int Xj, int Ui, int Uj, int Vi, int Vj, int n) {
	if (n == 16)
		A_loop_FW(Xi, Xj, Ui, Uj, Vi, Vj, n);

	else {
		AFW(Xi, Xj, Ui, Uj, Vi, Vj, n/2);

		cilk_spawn AFW(Xi, Xj + n/2, Ui, Uj, Vi, Vj + n/2, n/2);
		AFW(Xi + n/2, Xj, Ui + n/2, Uj, Vi, Vj, n/2);
		cilk_sync;
		
		AFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj, Vi, Vj + n/2, n/2);
		

		AFW(Xi + n/2, Xj + n/2, Ui + n/2, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		
		cilk_spawn AFW(Xi + n/2, Xj, Ui + n/2, Uj + n/2, Vi + n/2, Vj, n/2);
		AFW(Xi, Xj + n/2, Ui, Uj + n/2, Vi + n/2, Vj + n/2, n/2);
		cilk_sync;

		AFW(Xi, Xj, Ui, Uj + n/2, Vi + n/2, Vj, n/2);
	}
}

int edges;

int main(int argc, char *argv[])
{      
	char *arg_vertices = getenv("N_VERTICES");
	vertices = atoi(arg_vertices);
	
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

	AFW(0, 0, 0, 0, 0, 0, vertices);

	for(int i = 0 ; i < vertices; i++ ) 
	{
		cout << "\n";
		for(int j = 0 ; j< vertices ;j++ )
			cout << dist[i][j] << " " ;
	}

	return 0;
}
