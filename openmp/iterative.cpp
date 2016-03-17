#include<stdio.h>
#include<iostream>
#include<limits.h>
#include<algorithm>
#include "omp.h"
using namespace std;

/* maxVertices represents maximum number of vertices that can be present in the graph. */
#ifndef maxVertices
#define maxVertices   16
#endif
#define INF           INT_MAX-1

#define NUM_THREADS 16


/* Input Format: Graph is directed and weighted. First two integers must be number of vertices and edges 
   which must be followed by pairs of vertices which has an edge between them. 
 */

int min(int a,int b){return (a<b)?a:b;}

int dist[maxVertices][maxVertices];
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
void FloydWarshall(int vertices)
{
        
	for(int via=0;via<vertices;via++)
	{
		
		#pragma omp parallel for
		for(int from=0;from<vertices;from++)
        {
                #pragma omp parallel for
                for(int to=0;to<vertices;to++)
                {
                        if(from!=to && from!=via && to!=via)
						{
							dist[from][to] = min(dist[from][to],dist[from][via]+dist[via][to]);
						}
                        
                }
        }
   }
}


int edges;


int main(int argc, char *argv[])
{      
	   vertices=maxVertices;
	    /*initialize dist between all pairs as infinity*/
       init(vertices);
        /* vertices represent number of vertices and edges represent number of edges in the graph. */
		
omp_set_num_threads(NUM_THREADS);

#pragma omp parallel for
	for(int i = 0 ; i < vertices ; i++ )
       {

#pragma omp parallel for
          for(int j = 0 ; j< vertices; j++ )       
           {
                if( i == j )
					dist[i][j] = 0;
				else
                    dist[i][j] = i+j;
                
			}
		}
	

		FloydWarshall(vertices);
		

		for(int i = 0 ; i < vertices; i++ ) 
		{
			cout << "\n";
			for(int j = 0 ; j< vertices ;j++ )
				cout << dist[i][j] << " " ;
		}
	
        return 0;
}
