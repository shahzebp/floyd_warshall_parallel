#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <algorithm>
using namespace std;

/* maxVertices represents maximum number of vertices that can be present in the graph. */
#ifndef maxVertices
#define maxVertices   8192
#endif
#define INF           INT_MAX-1

double dist[maxVertices];

unsigned long long vertices;
unsigned long long m_val;

void swap(unsigned long long index1, unsigned long long index2)
{
	double temp;
	temp = dist[index1];
	dist[index1] = dist[index2];
	dist[index2] = temp;
}
void init()
{
	unsigned long long i;
	for(i=0;i<vertices;i++)
		dist[i] = i * 1.0;
	unsigned long long randomIndex1, randomIndex2;
	for(i=0;i<vertices;i++)
	{
		randomIndex1 = rand() % vertices;
		randomIndex2 = rand() % vertices;
		swap(randomIndex1, randomIndex2);
	}
}

void merge(unsigned long long l, unsigned long long m, unsigned long long r)
{
    unsigned long long i, j, k;
    unsigned long long n1 = m - l + 1;
    unsigned long long n2 =  r - m;
 
    double L[n1], R[n2];
 
    for (i = 0; i < n1; i++)
        L[i] = dist[l + i];
    for (j = 0; j < n2; j++)
        R[j] = dist[m + 1+ j];
 
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        if (L[i] <= R[j])
        {
            dist[k] = L[i];
            i++;
        }
        else
        {
            dist[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1)
    {
        dist[k] = L[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        dist[k] = R[j];
        j++;
        k++;
    }
}

bool verify_array()
{
	unsigned long long i;
	for(i=0;i<vertices-1;i++)
	{
		if(dist[i] < dist[i+1]) return false;
	}
	return true;	
}

void print_array()
{
	unsigned long long i;
	for(i=0;i<vertices;i++) cout << dist[i] << " ";
}

void insertionSort(unsigned long long l, unsigned long long r)
{
   unsigned long long i, j;
   double key;
   for (i = l + 1; i < r; i++)
   {
       key = dist[i];
       j = i-1;
 
       while (j >= l && dist[j] > key)
       {
           dist[j+1] = dist[j];
           j = j-1;
       }
       dist[j+1] = key;
   }
}

void ParMergeSortPM(unsigned long long p, unsigned long long r)
{
	unsigned long long n = r - p + 1;
	if(n <= m_val)
		insertionSort(p, p + n);
	else
	{
		unsigned long long q = (p + r)/2;
		ParMergeSortPM(p, q);
		ParMergeSortPM(q+1, r);
		merge(p, q, r);
	}
}
		
int main(int argc, char *argv[])
{
	char *arg_vertices = getenv("N_VERTICES");
	char *arg_m_val    = getenv("M_VAL");

	string plotFileName = "fwr1_" + string(arg_vertices) + "_" + string(arg_m_val);

	vertices = atoi(arg_vertices);
	m_val	 = atoi(arg_m_val);
	
	init();
	//cilk::cilkview cv;
        //cv.start();
	ParMergeSortPM(0, vertices-1);
	print_array();
	//AFW(0, 0, 0, 0, 0, 0, vertices);

        //cv.stop();
        //cv.dump(plotFileName.c_str(), false);

        //std::cout << cv.accumulated_milliseconds()  << " millseconds" << std::endl;
	
	return 0;
}
