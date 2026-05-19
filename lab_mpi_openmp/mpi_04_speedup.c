#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10000000

int main(int argc, char** argv) {
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_FUNNELED, &provided);

    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int chunk = N / size;
    long long *arr = NULL;
    long long *arr_local = (long long*)malloc(chunk * sizeof(long long));
    long long sum_local = 0;
    long long sum_total = 0;
    int num_hilos = 0;

    double t_inicio = MPI_Wtime(); 

    if (rank == 0) {
        arr = (long long*)malloc(N * sizeof(long long));
        for (int i = 0; i < N; i++) {
            *(arr + i) = i;
        }
    }

    MPI_Scatter(arr, chunk, MPI_LONG_LONG, arr_local, chunk, MPI_LONG_LONG, 0, MPI_COMM_WORLD);

    #pragma omp parallel
    {
        num_hilos = omp_get_num_threads();
        #pragma omp for reduction(+:sum_local)
        for (int i = 0; i < chunk; i++) {
            sum_local += *(arr_local + i);
        }
    }

    MPI_Reduce(&sum_local, &sum_total, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    double t_fin = MPI_Wtime();

    if (rank == 0) {
        double tiempo_paralelo = t_fin - t_inicio;
        printf("Usando %d procesos MPI con %d hilos por proceso\n",size,num_hilos);
        printf("Suma total = %lld\n", sum_total);
        printf("Tiempo paralelo: %.4f segundos\n", tiempo_paralelo);

        long long sum_seq = 0;
        double ts = MPI_Wtime();
        
        for (int i = 0; i < N; i++) {
            sum_seq += i;
        }
        
        double te = MPI_Wtime();
        double tiempo_secuencial = te - ts;

        printf("Tiempo secuencial: %.4f segundos\n", tiempo_secuencial);
        printf("Speedup: %.2fx\n", tiempo_secuencial / tiempo_paralelo);

        free(arr);
    }

    free(arr_local);
    MPI_Finalize();
    return 0;
}