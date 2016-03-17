#include<stdio.h>
#include<iostream>
#include <cstdlib>

#include<limits.h>
#include<algorithm>
#include<cilk/cilk.h>
#include <cilkview.h>

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
void FloydWarshall(int vertices)
{
        
	for(int via=0;via<vertices;via++)
	{
	cilk_for(int from=0;from<vertices;from++)
        {
                cilk_for(int to=0;to<vertices;to++)
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
	char *arg_vertices = getenv("N_VERTICES");
	vertices = atoi(arg_vertices);
        cilkview_data_t d;
	__cilkview_query(d);	
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


	FloydWarshall(vertices);
	__cilkview_report(&d, NULL, "fw", CV_REPORT_WRITE_TO_RESULTS);
	for(int i = 0 ; i < vertices; i++ ) 
	{
		cout << "\n";
		for(int j = 0 ; j< vertices ;j++ )
			cout << dist[i][j] << " " ;
	}

	return 0;
}
