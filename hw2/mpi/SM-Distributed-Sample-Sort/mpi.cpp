#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>
#include <set>
using namespace std;

extern "C++" int ParMergeSortSM_CPP(double *dist, unsigned long long p,
                        unsigned long long r);
double *dist;

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

bool verify_array(double *dist, unsigned long long size)
{
    unsigned long long i;
    for(i=0;i < size-1; i++)
    {   
        if(dist[i] > dist[i+1]) return false;
    }
    return true;
}

int main(int argc, char **argv) {

        char *arg_vertices = getenv("N_VERTICES");
	
        vertices = atoi(arg_vertices);

	dist = new double[vertices];
	int size, rank;

        int p = 5; 
	int q = 5;

        int keys_per_node = vertices/p; 

        double *es_keys = NULL;
        double *sub_es_keys = NULL;
        double *g_pivots = NULL;
	double *local_arr;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	local_arr = (double *)malloc(sizeof(double) * keys_per_node); 
	
	if (rank == 0) 
		init();

    	MPI_Scatter(dist, keys_per_node, MPI_DOUBLE, local_arr,
		keys_per_node, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	ParMergeSortSM_CPP(local_arr, 0, keys_per_node - 1);
	//sort(local_arr, local_arr + keys_per_node);

	sub_es_keys  =  (double *)malloc(sizeof(double) *(q - 1));

	int index = keys_per_node / q;

	for (int x = 0; x < q-1; x++) 
		sub_es_keys[x] = local_arr[(x + 1) * index];

	if (rank == 0) {
	        es_keys = (double *)malloc(p*(q-1) * sizeof(double));
	}

	MPI_Gather(sub_es_keys, q - 1, MPI_DOUBLE, es_keys, (q - 1),
			 MPI_DOUBLE, 0, MPI_COMM_WORLD);
	

	g_pivots = (double *)malloc((size - 1) * sizeof(double));

	if (rank == 0) {
	
		//sort(es_keys, es_keys  + (p * (q-1)));
		ParMergeSortSM_CPP(es_keys, 0, (p * (q-1)) - 1);
	
	        int stride = q - 1;

	        for (int i = 0; i < size - 1; i++) {
        	        g_pivots[i] = es_keys[(i + 1) * stride - 1];
		}
	}

	MPI_Bcast(g_pivots, size - 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

/*
	vector <vector <double> > ans; 

        vector <double> temp;

	int l = 0;
	int r = keys_per_node - 1;

	int i = 0;
	
	while (l <= r) {
                if (local_arr[l] < g_pivots[i] || i == size - 1) {
                        temp.push_back(local_arr[l]);
                        l++;
                }
                else {
                        temp.push_back(g_pivots[i]);

                        if (local_arr[l] == g_pivots[i])
                                l++;

                        ans.push_back(temp);
                        temp.resize(0);
                        i++;
                }
        }

        ans.push_back(temp);

        vector <vector <double> > ::iterator it;
        vector <double> ::iterator iter;

	double collate[keys_per_node][keys_per_node];
	memset(collate, -1, sizeof collate);

	for (int i = 0; i < size; i++) {
		if (rank != i) {
			MPI_Send(&ans[i].front(), ans[i].size(), MPI_DOUBLE, i,
			0, MPI_COMM_WORLD);
		} else {
			int j = 0;
		        vector <double> ::iterator it;

			for (it = ans[i].begin(); it != ans[i].end(); ++it){
				collate[i][j] = *it;
				j++;
			}
		}
		
	}

	for (int i = 0; i < size; i++) {
		if (rank != i) {
			MPI_Recv(&collate[i], keys_per_node, MPI_DOUBLE, i, 0,
			MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
	}

	vector<double> sorted;

	for (int i = 0; i < keys_per_node; i++)
	{
		for(int j = 0; j < keys_per_node; j++) {
			if (collate[i][j] > -1)
				sorted.push_back(collate[i][j]);
		}
	}	

	sort(sorted.begin(), sorted.end());

	set<double> uniq(sorted.begin(), sorted.end());

	vector<double> data(uniq.begin(), uniq.end());

	int *counts = new int[size];
	int nelements = data.size();

	MPI_Gather(&nelements, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int *disps = new int[size];

	for (int i = 0; i < size; i++)
		disps[i] = (i > 0) ? (disps[i-1] + counts[i-1]) : 0;

	double *alldata = NULL;
	if (rank == 0)
  		alldata = new double[disps[size-1]+counts[size-1]];


	MPI_Gatherv(&data[0], nelements, MPI_DOUBLE,
            alldata, counts, disps, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		if (false == verify_array(alldata, vertices))
			cout << "Alert: Not Sorted " << endl;
		else
			cout << " Sort Successfull " << endl << endl;
	}
*/
	MPI_Finalize();

	return 0;
}
