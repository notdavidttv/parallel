#include<stdio.h>
#include<mpi.h>

int main(int argc, char **argv){
    MPI_Init(&argc,&argv);
    int rank;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    printf("Proceso %d de %d: ¡Hola desde MPI!\n",rank,size);

    if(rank==0){
        printf("[MAESTRO] Todos los %d procesos han saludado.",size);
    }

    MPI_Finalize();
    return 0;
}