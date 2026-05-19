#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 1000000

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int size;
    int rank;

    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int chunk= N/size;

    long long *arr=NULL;
    long long *arr_local=(long long*)malloc(chunk*sizeof(long long));
    long long sum_local=0;

    if(rank==0){
        arr=(long long*)malloc(N*sizeof(long long));
        for(int i=0; i<N;i++){
            *(arr + i)=i;
        }
    }

    MPI_Scatter(arr,chunk,MPI_LONG_LONG,arr_local,chunk,MPI_LONG_LONG,0,MPI_COMM_WORLD);

    #pragma omp parallel for reduction(+:sum_local)
    
        for(int i=0;i<chunk;i++){
            sum_local+= *(arr_local + i);
        }
    
    long long sum_total=0;
    MPI_Reduce(&sum_local,&sum_total,1,MPI_LONG_LONG,MPI_SUM,0,MPI_COMM_WORLD);

        if (rank == 0) {
        printf("Suma total = %lld\n", sum_total);
        printf("Esperado  = %lld\n", (long long)N*(N-1)/2);
        free(arr);
    }

    free(arr_local);
    MPI_Finalize();
    return 0;
    
}