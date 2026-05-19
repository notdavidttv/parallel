    #include<mpi.h>
    #include<omp.h>
    #include<stdio.h>

    int main(int argc, char **argv){
        int provided;
        MPI_Init_thread(&argc,&argv, MPI_THREAD_FUNNELED,&provided);

        int size;
        int rank;

        MPI_Comm_size(MPI_COMM_WORLD,&size);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

        #pragma omp parallel num_threads(4)
        {
            int thread_id=omp_get_thread_num();
            int total_threads=omp_get_num_threads();

            printf("  Proceso MPI %d | Hilo OpenMP %d de %d\n",rank,thread_id,total_threads);
        }

        if(rank ==0){
            printf("Total unidades: %d x 4 = %d\n", size, size * 4);
        }

        MPI_Finalize();
        return 0;
    }