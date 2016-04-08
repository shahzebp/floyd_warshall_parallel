#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "mpi.h"

using namespace std;

extern "C++" int ParMergeSortPM_CPP(double *dist, unsigned long long p,
                        unsigned long long r);

unsigned long long vertices;
unsigned long long m_val;

void swap(double *dist, unsigned long long index1, unsigned long long index2)
{
        double temp;
        temp = dist[index1];
        dist[index1] = dist[index2];
        dist[index2] = temp;
}

void Fill_Array(double *dist)
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

bool verify_array(double *dist, unsigned long long size)
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
        struct timeval tvalBefore, tvalAfter;


	int 	     size,rank, master = 0;
	unsigned long long 	     i,j,k, elem_size, elem_size_local,
			  elem_to_sort, count, temp;
	
	double 	     *arr, *per_proc_arr;
	double 	     *es_keys, *gs_pivots;
	double 	     *buckets, *bucket_arr, *per_proc_bucket;
	double 	     *sorted_arr, *final_arr;

	double 	     *temp_per_proc_arr, *temp_gs_pivots, *temp_per_proc_bucket;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == master){

		elem_size = atoi(argv[1]);

		arr = new double[elem_size];

		vertices = elem_size;

		Fill_Array(arr);
	
		gettimeofday (&tvalBefore, NULL);
	}


	MPI_Bcast (&elem_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	elem_size_local = elem_size / size;
	per_proc_arr = new double[elem_size_local];
	temp_per_proc_arr = new double[elem_size_local];

	MPI_Scatter(arr, elem_size_local, MPI_INT, per_proc_arr, 
			  elem_size_local, MPI_INT, master, MPI_COMM_WORLD);

        struct timeval *innerBefore = new struct timeval();
	
	if (rank == master){
		gettimeofday (innerBefore, NULL);
	}


	ParMergeSortPM_CPP(per_proc_arr, temp_per_proc_arr, 0, elem_size_local - 1);

	es_keys = new double[(size-1)];

	for (i=0; i< (size-1); i++){
		es_keys[i] = per_proc_arr[elem_size/(size*size) * (i+1)];
	} 

	gs_pivots = new double[size * (size-1)];
	temp_gs_pivots = new double[size * (size-1)];

	MPI_Gather (es_keys, size-1, MPI_INT, gs_pivots, size-1, 
			  MPI_INT, master, MPI_COMM_WORLD);

	if (rank == master){
		ParMergeSortPM_CPP(gs_pivots, temp_gs_pivots, 0, size*(size-1) - 1);
		for (i=0; i<size-1; i++)
			es_keys[i] = gs_pivots[(size-1)*(i+1)];
	}

	MPI_Bcast (es_keys, size-1, MPI_INT, 0, MPI_COMM_WORLD);

	buckets = new double[elem_size + size];;

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

	bucket_arr = new double[elem_size + size];

	MPI_Alltoall (buckets, elem_size_local + 1, MPI_INT, bucket_arr, 
				 elem_size_local + 1, MPI_INT, MPI_COMM_WORLD);

	per_proc_bucket = new double[2 * elem_size / size];
	temp_per_proc_bucket = new double[2 * elem_size / size];

	count = 1;

	for (j=0; j<size; j++) {
		k = 1;
		for (i=0; i<bucket_arr[(elem_size/size + 1) * j]; i++) 
			per_proc_bucket[count++] = 
				bucket_arr[(elem_size/size + 1) * j + k++];
	}

	per_proc_bucket[0] = count-1;

	elem_to_sort = per_proc_bucket[0];
	
	ParMergeSortPM_CPP(&per_proc_bucket[1], &temp_per_proc_bucket[1], 0, elem_to_sort - 1);

	if(rank == master) {
		sorted_arr = new double[2 * elem_size];
		final_arr = new double[elem_size];

        	struct timeval *innerAfter = new struct timeval();

		gettimeofday (innerAfter, NULL);
			
		printf("Inner Time: %ld microseconds\n",
            		((innerAfter->tv_sec - innerBefore->tv_sec)*1000000L
           		+ innerAfter->tv_usec) - innerBefore->tv_usec);
		

	}

	MPI_Gather (per_proc_bucket, 2*elem_size_local, MPI_INT, sorted_arr, 
			  2*elem_size_local, MPI_INT, master, MPI_COMM_WORLD);

	if (rank == master){
		count = 0;
		for(j=0; j<size; j++){
  			k = 1;
 			for(i=0; i<sorted_arr[(2 * elem_size/size) * j]; i++) 
			  final_arr[count++] = sorted_arr[(2*elem_size/size) * j + k++];
		}

		gettimeofday (&tvalAfter, NULL);
		
		printf("Time: %ld microseconds\n",
            		((tvalAfter.tv_sec - tvalBefore.tv_sec)*1000000L
           		+tvalAfter.tv_usec) - tvalBefore.tv_usec
          		);

		if (false == verify_array(final_arr, elem_size))
			cout << "Alert:  sort went wrong " << endl;
		else
			cout << "Sort Successfull " << endl;


	}

	MPI_Finalize();
}



