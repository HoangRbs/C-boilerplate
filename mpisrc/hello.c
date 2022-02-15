#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

// mpicx file.cpp
// mpirun -np 3 ./a.out 

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
    int num_process, processId;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   
    MPI_Status thongbao;


    printf("hello from process %d trong tat ca %d cpu \n", processId, num_process);

    MPI_Finalize();
    return 0;
}