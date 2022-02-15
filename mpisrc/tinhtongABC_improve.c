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

    MPI_Scatter(A, ms, MPI_INT, As, ms, MPI_INT , 0, MPI_COMM_WORLD);
    MPI_Scatter(B, ms, MPI_INT, Bs, ms, MPI_INT , 0, MPI_COMM_WORLD);

    // all processes calculate Cs = As + Bs;
    for (int i = 0; i < ms; i++) {
        Cs[i] = As[i] + Bs[i];
    }

    // all process send Cs to process0 and merge into C
    MPI_Gather(Cs, ms, MPI_INT, C, ms, MPI_INT, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        // in ra
        printf("C: ");
        for (int i = 0; i < m; i++) {
            printf("%d ", C[i]); 
        }
    }

    MPI_Finalize();
    return 0;
}


// MPI_Scatter( const void* sendbuf , int sendcount , MPI_Datatype sendtype , void* recvbuf , int recvcount , MPI_Datatype recvtype , int root , MPI_Comm comm);
/*
    lúc send thì luôn send từ process 0 tới tất cả các process
    nên dữ liệu send *sendbuf chỉ cần khởi tạo trong process 0
    hay nói là: nó chỉ lấy dữ liệu send là mảng C mà ở trong process 0

    còn *recvbuf thì là các dữ liệu mảng được tồn tại trong tất cả các process 0,1,2 , VD: cs phải có ở trong tất cá process 0,1,2
*/


// MPI_Gather( const void* sendbuf , int sendcount , MPI_Datatype sendtype , void* recvbuf , int recvcount , MPI_Datatype recvtype , int root , MPI_Comm comm);
/*
    còn  gather thì gather tất cả sendbuf từ tất cả các process
    nên sendbuf phải tồn tại trong tất cả các process

    còn recvbuf thì chỉ cần tồn tại trong process 0 vì lúc nhận thì chỉ merge vào process 0. VD: màng C chỉ cần ở trong process 0.
    hay nói là: nó chỉ effect mảng C mà ở trong process 0.
*/