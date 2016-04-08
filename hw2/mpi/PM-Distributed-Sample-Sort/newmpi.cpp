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

void init(int *dist)
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

	int 	     size,rank, Root = 0;
	int 	     i,j,k, elem_size, elem_size_local,
			  NoElementsToSort;
	int 	     count, temp;
	int 	     *Input, *InputData;
	int 	     *Splitter, *AllSplitter;
	int 	     *Buckets, *BucketBuffer, *LocalBucket;
	int 	     *OutputBuffer, *Output;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if(argc != 2) {
	if(rank ==0) printf(" Usage : run size\n");
		MPI_Finalize();
		 exit(0);
	}

	if (rank == Root){

		elem_size = atoi(argv[1]);

		Input = new int[elem_size];

		vertices = elem_size;
		init(Input);
	}

	MPI_Bcast (&elem_size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	elem_size_local = elem_size / size;
	InputData = (int *) malloc (elem_size_local * sizeof (int));

	MPI_Scatter(Input, elem_size_local, MPI_INT, InputData, 
			  elem_size_local, MPI_INT, Root, MPI_COMM_WORLD);

	ParMergeSortSM_CPP(InputData, 0, elem_size_local - 1);
	

	Splitter = (int *) malloc (sizeof (int) * (size-1));
	for (i=0; i< (size-1); i++){
		Splitter[i] = InputData[elem_size/(size*size) * (i+1)];
	} 

	AllSplitter = (int *) malloc (sizeof (int) * size * (size-1));
	MPI_Gather (Splitter, size-1, MPI_INT, AllSplitter, size-1, 
			  MPI_INT, Root, MPI_COMM_WORLD);

	if (rank == Root){
		ParMergeSortSM_CPP(AllSplitter,0, size*(size-1) - 1);
		for (i=0; i<size-1; i++)
			Splitter[i] = AllSplitter[(size-1)*(i+1)];
	}

	MPI_Bcast (Splitter, size-1, MPI_INT, 0, MPI_COMM_WORLD);

	Buckets = (int *) malloc (sizeof (int) * (elem_size + size));

	j = 0;
	k = 1;

	for (i=0; i<elem_size_local; i++){
		if(j < (size-1)){
			if (InputData[i] < Splitter[j]) 
		 		Buckets[((elem_size_local + 1) * j) + k++] = InputData[i]; 
			else{
 	  	      	Buckets[(elem_size_local + 1) * j] = k-1;
	    		k=1;
		 	j++;
		        i--;
			}	
		}
		else 
			Buckets[((elem_size_local + 1) * j) + k++] = InputData[i];
	}

	Buckets[(elem_size_local + 1) * j] = k - 1;

	BucketBuffer = (int *) malloc (sizeof (int) * (elem_size + size));

	MPI_Alltoall (Buckets, elem_size_local + 1, MPI_INT, BucketBuffer, 
				 elem_size_local + 1, MPI_INT, MPI_COMM_WORLD);

	LocalBucket = (int *) malloc (sizeof (int) * 2 * elem_size / size);

	count = 1;

	for (j=0; j<size; j++) {
		k = 1;
		for (i=0; i<BucketBuffer[(elem_size/size + 1) * j]; i++) 
			LocalBucket[count++] = 
				BucketBuffer[(elem_size/size + 1) * j + k++];
	}
	LocalBucket[0] = count-1;

	NoElementsToSort = LocalBucket[0];
	
	ParMergeSortSM_CPP(&LocalBucket[1], 0, NoElementsToSort - 1);

	if(rank == Root) {
		OutputBuffer = (int *) malloc (sizeof(int) * 2 * elem_size);
		Output = (int *) malloc (sizeof (int) * elem_size);
	}

	MPI_Gather (LocalBucket, 2*elem_size_local, MPI_INT, OutputBuffer, 
			  2*elem_size_local, MPI_INT, Root, MPI_COMM_WORLD);

	if (rank == Root){
		count = 0;
		for(j=0; j<size; j++){
  			k = 1;
 			for(i=0; i<OutputBuffer[(2 * elem_size/size) * j]; i++) 
			  Output[count++] = OutputBuffer[(2*elem_size/size) * j + k++];
		}

		if (false == verify_array(Output, elem_size))
			cout << "Alert:  sort went wrong " << endl;
		else
			cout << "Sort Successfull " << endl;

		free(Input);
		free(OutputBuffer);
		free(Output);
	}

	free(InputData);
	free(Splitter);
	free(AllSplitter);
	free(Buckets);
	free(BucketBuffer);
	free(LocalBucket);

	MPI_Finalize();
}



