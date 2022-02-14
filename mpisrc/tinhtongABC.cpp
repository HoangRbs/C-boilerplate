#include <stdio.h>
#include <iostream>
#include <mpi.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

using namespace std;

int main (int argc, char **argv) {
    MPI_Init(&argc, &argv); // must have argument so the command from terminal can pass arguments into program
    
    int num_process, id_process;
    MPI_Status thongbao;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &id_process);     // get OUT id_process

    // printf("hello from process %d trong tat ca %d cpu \n", id_process, num_process);

    // if (id_process == 0) {
    //     int *A = new int[5];
    //     for (int i = 0; i < 5; i++) {
    //         A[i] = rand() % 10 + 1;
    //         cout << A[i] << " "; 
    //     }
    //     cout << endl;

    //     MPI_Send(A,5,MPI_INT, 1, 3112021, MPI_COMM_WORLD);
    // } else {
    //     int *B, *C;
    //     B = new int[5];
    //     C = new int[5];
    //     MPI_Recv(B,5,MPI_INT, 0, 3112021, MPI_COMM_WORLD, &thongbao);

    //     for (int i = 0; i < 5; i++) {
    //         C[i] = 2 * B[i];
    //         cout << C[i] << " ";
    //     }
    //     cout << endl;
    // }

    // outside of scope datas will be created and calculated for all processes
    // bai tap: tinh tong

    int m = 6;      // numProcess = 3 just take 6 / 3 = 2 elements per process for simplification.

    int *A = new int[m];
    int *B = new int[m];
    int *C = new int[m];

    int ms = m / num_process;   // elements per process 6 / 3 = 2

    int *As = new int[ms];
    int *Bs = new int[ms];
    int *Cs = new int[ms];
        

    if (id_process == 0) {
        for (int i = 0; i < m; i++) {
            A[i] = rand() % 10 + 1;
            B[i] = rand() % 10 + 1;
        }

        // in ra
        cout << "A: ";
        for (int i = 0; i < m; i++) {
            cout << A[i] << " "; 
        }
        cout << endl;
        // in ra
        cout << "B: ";
        for (int i = 0; i < m; i++) {
            cout << B[i] << " "; 
        }
        cout << endl;
    }

    // --> improved version using scatter
    /*
    if (id_process == 0) {

        for (int i = 0; i < ms; i++) {
            As[i] = A[i];
            Bs[i] = B[i];
        }

        // send elements to other processes 1,2
        for (int cpu = 1; cpu < num_process; cpu++) {
            MPI_Send(A + (cpu * ms), ms,MPI_INT, cpu, cpu, MPI_COMM_WORLD);
            MPI_Send(B + (cpu * ms), ms,MPI_INT, cpu, cpu+100, MPI_COMM_WORLD);
        }
        
    } else {    // cac process con lai

        MPI_Recv(As, ms, MPI_INT, 0, id_process, MPI_COMM_WORLD, &thongbao);
        MPI_Recv(Bs, ms, MPI_INT, 0, id_process+100, MPI_COMM_WORLD, &thongbao);
    } */
    

    // --> improved version using scatter
    MPI_Scatter(A, ms, MPI_INT, As, ms, MPI_INT , 0, MPI_COMM_WORLD);
    MPI_Scatter(B, ms, MPI_INT, Bs, ms, MPI_INT , 0, MPI_COMM_WORLD);

    // all processes calculate Cs = As + Bs;
    for (int i = 0; i < ms; i++) {
        Cs[i] = As[i] + Bs[i];
    }

    // --> improved version using Gather().
    // gui ket qua Cs ve process 0
    // if (id_process != 0) {
    //     MPI_Send(Cs, ms, MPI_INT, 0, id_process+200, MPI_COMM_WORLD);
    // }

    // process 0 nhan ket qua Cs tu nhieu process
    // if (id_process == 0) {
    //     // save Cs into process 0 into C
    //     for (int i =0; i < ms; i++) {
    //         C[i] = Cs[i]; 
    //     }

    //     // save Cs from other process into C
    //     for (int cpu = 1; cpu < num_process; cpu++) {
    //         MPI_Recv(C + (cpu * ms), ms, MPI_INT, cpu, cpu+200, MPI_COMM_WORLD, &thongbao);
    //     }

    //     // in ra
    //     cout << "C: ";
    //     for (int i = 0; i < m; i++) {
    //         cout << C[i] << " "; 
    //     }
    // }

    // --> improved using gather
    // all process send Cs into C
    MPI_Gather(Cs, ms, MPI_INT, C, ms, MPI_INT, 0, MPI_COMM_WORLD);

    if (id_process == 0) {
        // in ra
        cout << "C: ";
        for (int i = 0; i < m; i++) {
            cout << C[i] << " "; 
        }
    }

    MPI_Finalize();
    return 0;
}

// mpicxx file.cpp
// mpirun -np 3 ./a.out 