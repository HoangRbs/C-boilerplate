#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


// mpicc file.c
// mpirun -np 3 ./a.out 

int main(int argc, char *argv[]) {
    srand (time(NULL));

    MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
    int num_process, processId;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   
    MPI_Status thongbao;

    // bai tap tinh tong

    int m = 6;      // numProcess = 3 just take 6 / 3 = 2 elements per process
    int ms = m / num_process;   // elements per process: 6 / 3 = 2

    int *As = malloc(ms * sizeof(int));
    int *Bs = malloc(ms * sizeof(int));
    int *Cs = malloc(ms * sizeof(int));

    int *A = malloc(m * sizeof(int));
    int *B = malloc(m * sizeof(int));
    int *C = malloc(m * sizeof(int));   // C la ket qua

    // when outside of process id scope, all above datas is copied to all processes    

    // init
    if (processId == 0) {
        // khoi tao A, B
        for (int i = 0; i < m; i++) {
            A[i] = rand() % 10 + 1;
            B[i] = rand() % 10 + 1;
        }

        // in ra
        printf("A: ");
        for (int i = 0; i < m; i++) {
            printf("%d ", A[i]); 
        }
        printf("\n");
        
        // in ra
        printf("B: ");
        for (int i = 0; i < m; i++) {
            printf("%d ", B[i]); 
        }
        printf("\n");
    }


    if (processId == 0) {
        // init As, Bs of process 0
        for (int i = 0; i < ms; i++) {
            As[i] = A[i];
            Bs[i] = B[i];
        }

        // send elements from 0 to processes 1,2 to init their own As, Bs
        for (int cpu = 1; cpu < num_process; cpu++) {
            MPI_Send(A + (cpu * ms), ms,MPI_INT, cpu, cpu, MPI_COMM_WORLD);
            MPI_Send(B + (cpu * ms), ms,MPI_INT, cpu, cpu+100, MPI_COMM_WORLD);
        }
        
    } else {    // cac process con lai

        // receive 
        MPI_Recv(As, ms, MPI_INT, 0, processId, MPI_COMM_WORLD, &thongbao);
        MPI_Recv(Bs, ms, MPI_INT, 0, processId+100, MPI_COMM_WORLD, &thongbao);
    }

    // all processes calculate Cs = As + Bs;
    for (int i = 0; i < ms; i++) {
        Cs[i] = As[i] + Bs[i];
    }

    // gui ket qua Cs cua process 1 va 2 ve process 0
    if (processId != 0) {
        MPI_Send(Cs, ms, MPI_INT, 0, processId+200, MPI_COMM_WORLD);
    }

    // process 0 nhan ket qua Cs tu nhieu process
    if (processId == 0) {
        // save Cs of process 0 into C first
        for (int i =0; i < ms; i++) {
            C[i] = Cs[i]; 
        }

        // receive Cs of process 1 and 2 and save into C
        for (int cpu = 1; cpu < num_process; cpu++) {
            MPI_Recv(C + (cpu * ms), ms, MPI_INT, cpu, cpu+200, MPI_COMM_WORLD, &thongbao);
        }

        // in ra
        printf("C: ");
        for (int i = 0; i < m; i++) {
            printf("%d ", C[i]); 
        }
    }

    MPI_Finalize();
    return 0;
}