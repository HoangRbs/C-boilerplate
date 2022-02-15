#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

// chay 2 process

// mpicx file.cpp
// mpirun -np 2 ./a.out 

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
    int num_process, processId;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   
    MPI_Status thongbao;

    if (processId == 0) {
        int *A = malloc(5 * sizeof(int));
        for (int i = 0; i < 5; i++) {
            A[i] = rand() % 10 + 1;
            printf("%d ", A[i]);
        }
        printf("\n");

        MPI_Send(A,5,MPI_INT, 1, 3112021, MPI_COMM_WORLD);
        
        // MPI_Send( const void* buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm);
        // in order to be able to sent and recv: focus on
        // dest: destination process Id
        // tag: message tag (should be similar to recv)
    } else {
        int *B, *C;
        B = malloc(5 * sizeof(int));
        C = malloc(5 * sizeof(int));

        MPI_Recv(B,5,MPI_INT, 0, 3112021, MPI_COMM_WORLD, &thongbao);
        // MPI_Recv( void* buf , int count , MPI_Datatype datatype , int source , int tag , MPI_Comm comm , MPI_Status* status);
        // source: source process id that sent the buff
        // message tag (should be similar to sent)

        for (int i = 0; i < 5; i++) {
            C[i] = 2 * B[i];
            printf("%d ", C[i]);
        }
        printf("\n");
    }

    MPI_Finalize();
    return 0;
}