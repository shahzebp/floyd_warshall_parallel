#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpi.h"

using namespace std;

extern "C++" int ParMergeSortSM_CPP(int *dist, unsigned long long p,
                        unsigned long long r);

unsigned long long vertices;
unsigned long long m_val;

void swap(int *dist, unsigned long long index1, unsigned long long index2)
{
        int temp;
        temp = dist[index1];
        dist[index1] = dist[index2];
        dist[index2] = temp;
}

void Fill_Array(int *dist)
{
        unsigned long long i;
        for(i=0;i<vertices;i++)
                dist[i] = i * 1.0;
        unsigned long long randomIndex1, randomIndex2;
        for(i=0;i<vertices;i++)
        {
                randomIndex1 = rand() % vertices;
                randomIndex2 = rand() % vertices;
                swap(dist, randomIndex1, randomIndex2);
        }
}

bool verify_array(int *dist, unsigned long long size)
{
    unsigned long long i;
    for(i=0;i < size-1; i++)
    {
        if(dist[i] > dist[i+1]) return false;
    }
    return true;
}


int main (int argc, char *argv[])
{
	int 	     size,rank, master = 0;
	int 	     i,j,k, elem_size, elem_size_local,
			  elem_to_sort;
	int 	     count, temp;
	int 	     *arr, *per_proc_arr;
	int 	     *es_keys, *gs_pivots;
	int 	     *buckets, *bucket_arr, *LocalBucket;
	int 	     *sorted_arr, *final;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == master){

		elem_size = atoi(argv[1]);

		arr = new int[elem_size];

		vertices = elem_size;

		Fill_Array(arr);
	}

	MPI_Bcast (&elem_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	elem_size_local = elem_size / size;
	per_proc_arr = new int[elem_size_local];

	MPI_Scatter(arr, elem_size_local, MPI_INT, per_proc_arr, 
			  elem_size_local, MPI_INT, master, MPI_COMM_WORLD);

	ParMergeSortSM_CPP(per_proc_arr, 0, elem_size_local - 1);
	

	es_keys = new int[(size-1)];
	for (i=0; i< (size-1); i++){
		es_keys[i] = per_proc_arr[elem_size/(size*size) * (i+1)];
	} 

	gs_pivots = new int[size * (size-1)];

	MPI_Gather (es_keys, size-1, MPI_INT, gs_pivots, size-1, 
			  MPI_INT, master, MPI_COMM_WORLD);

	if (rank == master){
		ParMergeSortSM_CPP(gs_pivots,0, size*(size-1) - 1);
		for (i=0; i<size-1; i++)
			es_keys[i] = gs_pivots[(size-1)*(i+1)];
	}

	MPI_Bcast (es_keys, size-1, MPI_INT, 0, MPI_COMM_WORLD);

	buckets = (int *) malloc (sizeof (int) * (elem_size + size));

	j = 0;
	k = 1;

	for (i=0; i<elem_size_local; i++){
		if(j < (size-1)){
			if (per_proc_arr[i] < es_keys[j]) 
		 		buckets[((elem_size_local + 1) * j) + k++] = per_proc_arr[i]; 
			else{
 	  	      	buckets[(elem_size_local + 1) * j] = k-1;
	    		k=1;
		 	j++;
		        i--;
			}	
		}
		else 
			buckets[((elem_size_local + 1) * j) + k++] = per_proc_arr[i];
	}

	buckets[(elem_size_local + 1) * j] = k - 1;

	bucket_arr = new int[elem_size + size];

	MPI_Alltoall (buckets, elem_size_local + 1, MPI_INT, bucket_arr, 
				 elem_size_local + 1, MPI_INT, MPI_COMM_WORLD);

	LocalBucket = new int[2 * elem_size / size];

	count = 1;

	for (j=0; j<size; j++) {
		k = 1;
		for (i=0; i<bucket_arr[(elem_size/size + 1) * j]; i++) 
			LocalBucket[count++] = 
				bucket_arr[(elem_size/size + 1) * j + k++];
	}

	LocalBucket[0] = count-1;

	elem_to_sort = LocalBucket[0];
	
	ParMergeSortSM_CPP(&LocalBucket[1], 0, elem_to_sort - 1);

	if(rank == master) {
		sorted_arr = new int[2 * elem_size];
		final = new int[elem_size];
	}

	MPI_Gather (LocalBucket, 2*elem_size_local, MPI_INT, sorted_arr, 
			  2*elem_size_local, MPI_INT, master, MPI_COMM_WORLD);

	if (rank == master){
		count = 0;
		for(j=0; j<size; j++){
  			k = 1;
 			for(i=0; i<sorted_arr[(2 * elem_size/size) * j]; i++) 
			  final[count++] = sorted_arr[(2*elem_size/size) * j + k++];
		}

		if (false == verify_array(final, elem_size))
			cout << "Alert:  sort went wrong " << endl;
		else
			cout << "Sort Successfull " << endl;

	}

	MPI_Finalize();
}



