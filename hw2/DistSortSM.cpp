#include <stdio.h>
#include <iostream>
#include <limits.h>
#include <algorithm>
#include <vector>

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

void merge(double *dist, unsigned long long l, unsigned long long m, unsigned long long r)
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


void merge_write(vector <vector <double> > to_merge) {

        if (to_merge.size() == 0)
                return;

        for(int i = 1; i < to_merge.size(); i++) {
                to_merge[0].insert(to_merge[0].end(), to_merge[i].begin(), to_merge[i].end());
        }

	sort(to_merge[0].begin(), to_merge[0].end());

	to_merge[0].erase( unique( to_merge[0].begin(),
			to_merge[0].end() ), to_merge[0].end() );

	

}


void insertionSort(double *dist, unsigned long long l, unsigned long long r)
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

void ParMergeSortPM(double *dist, unsigned long long p, unsigned long long r)
{
	unsigned long long n = r - p + 1;
	if(n <= m_val)
		insertionSort(dist, p, p + n);
	else
	{
		unsigned long long q = (p + r)/2;
		ParMergeSortPM(dist, p, q);
		ParMergeSortPM(dist, q+1, r);
		merge(dist, p, q, r);
	}
}

vector <vector <double> > insert_g_pivots(double *arr, int l, int r, double *g_pivots,
					int g_size) {

        vector <vector <double> > ans;
        vector <double> temp;
        int i = 0;

        while (l <= r) {
                if (arr[l] < g_pivots[i] || i == g_size) {
                        temp.push_back(arr[l]);
			l++;
                }
                else {
                        temp.push_back(g_pivots[i]);
	
			if (arr[l] == g_pivots[i])
				l++;
	
                        ans.push_back(temp);
                        temp.resize(0);
                        i++;
                }
        }
        ans.push_back(temp);
        return ans;
}

void DISTRIBUTED_SAMPLE_SORT_SM(double *arr, int l, int r) {
        
	int p = 3; // num_of_processors(); // assume power of 2

        int keys_per_node = (r - l + 1)/p; // N = r-p+1 is also power of 2

        // q computation

        int x = 1, q = x;

        while (x < keys_per_node) {
                if (!((keys_per_node + 1) % (x + 1)))
                        q = (keys_per_node + 1) / (x + 1);
                x++;
        }

        /* master node collects even spaced keys from all nodes in this es_keys

        array maintained here */

        double *es_keys = NULL;
        es_keys = (double *)malloc(p*(q-1) * sizeof(double));


        double *g_pivots = NULL; // global pivots
        g_pivots = (double *)malloc((p - 1) * sizeof(double));


        /*step 1: initial distribution among p cores*/

        for(int i = 0; i < p; i++) {
                int l = i * keys_per_node;
                int r = (i+1) * keys_per_node - 1;

                /*step 2.1: each node sorts it's local keys*/
                ParMergeSortPM(arr, l, r);
                /*step 2.2: selects q-1 even spaced keys*/
                int sub_len = (r - l + 2) / q;
                for (int x = 0; x < q-1; x++) {
                        es_keys[((q-1)*i) + x] = arr[l - 1 + sub_len*(x+1)];
                }
	}

        /*step 2.3: sort even spaced keys from all nodes in master node*/

        ParMergeSortPM(es_keys, 0, p*(q-1)-1);

        /*step 2.4: selects p-1 global pivots from es_keys*/

        int stride = q-1, sp = -1;
        for (int i = 0; i < p-1; i++)
                g_pivots[i] = es_keys[(i + 1) * stride - 1];

        vector <vector <vector <double> > > v_matrix;
	
        for (int i = 0; i < p; i++)
                v_matrix.push_back(insert_g_pivots(arr, i*keys_per_node,
				(i+1)*keys_per_node - 1, g_pivots, p - 1));
	
        int start = 0;
        for (int i = 0; i < p; i++) {
                vector <vector <double> > to_merge;
                int size = 0;
                for (int j = 0; j < p; j++) {
                        size += (v_matrix[j][i]).size();

                        to_merge.push_back(v_matrix[j][i]);
                }

                merge_write(to_merge);
                start = start + size;
        } 
}
		
int main(int argc, char *argv[])
{
	//char *arg_vertices = getenv("N_VERTICES");
	//char *arg_m_val    = getenv("M_VAL");

	//string plotFileName = "fwr1_" + string(arg_vertices) + "_" + string(arg_m_val);

	vertices = 24; //atoi(arg_vertices);
	m_val	 =  4;//atoi(arg_m_val);
	
	init();
	//cilk::cilkview cv;
        //cv.start();
	//ParMergeSortPM(0, vertices-1);
	DISTRIBUTED_SAMPLE_SORT_SM(dist, 0, vertices-1);

	print_array();

        //cv.stop();
        //cv.dump(plotFileName.c_str(), false);

        //std::cout << cv.accumulated_milliseconds()  << " millseconds" << std::endl;
	
	return 0;
}
