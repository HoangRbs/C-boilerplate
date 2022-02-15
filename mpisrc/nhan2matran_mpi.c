#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// mpicc file.cpp
// mpirun -np 3 ./a.out 

// Nhap cac kich thuoc cua ma tran
#define  m       3
#define  n       3
#define  p       5

void DisplayMatrix(int *A, int row,  int col){
  int i,j;
  for(i=0;i<row;i++){
    for(j=0;j<col;j++) printf("%d\t",*(A+i*col+j));
    printf("\n");
  }
}

int main(int argc, char *argv[]) {
    srand (time(NULL));
    
    MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
    int num_process, processId;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   
    MPI_Status thongbao;

    // Khai bao ma tran A, B, C
    int* A = (int *) malloc ((m*n)*sizeof(int));
    int* B = (int *) malloc ((n*p)*sizeof(int));
    int* C = (int *) malloc ((m*p)*sizeof(int));

    int ms = m / num_process; // m = 6, num_process = 3 hoac laf m = 3, num_process = 3
    
    int* As = (int *) malloc ((ms*n)*sizeof(int));
    // int* Bs = (int *) malloc ((n*p)*sizeof(int)); no need since Bs === B
    int* Cs = (int *) malloc ((ms*p)*sizeof(int));

    // init
    if (processId == 0) {
        // Khoi Tao Du Lieu (Ma tran A, B)
        //Ma tran A la ma tran Don vi
        for (int i = 0 ; i < m ; i++ )
            for (int j = 0 ; j < n ; j++ ){
                if (i==j) 
                    *(A+i*n+j) = 1.;
                else
                    *(A+i*n+j) = 0;
            }

        printf("The matrix A:\n");
        DisplayMatrix(A, m, n);
        
        //Ma tran B
        for (int i = 0 ; i < n ; i++ )
            for (int j = 0 ; j < p ; j++ ){
                *(B+i*p+j) = 1*(i*p+j);
            }
        
        printf("The matrix B\n");
        DisplayMatrix(B, n, p);
    }

    if (processId == 0) {
        // init As of process 0
        for (int i = 0 ; i < ms ; i++ )
            for (int j = 0 ; j < n ; j++ ){
                As[i*n + j] = A[i*n + j];
            }

        for (int cpu = 1; cpu < num_process; cpu++) {
            // send elements A from 0 to processes 1,2 to init their own As
            MPI_Send(A+((cpu * ms) * n), ms * n, MPI_INT, cpu, cpu, MPI_COMM_WORLD);
            // send B
            MPI_Send(B, n*p, MPI_INT, cpu, cpu+100, MPI_COMM_WORLD);
        }
        
    } else {    // cac process con lai

        // receive 
        MPI_Recv(As, ms * n, MPI_INT, 0, processId, MPI_COMM_WORLD, &thongbao);
        MPI_Recv(B, n*p, MPI_INT, 0, processId+100, MPI_COMM_WORLD, &thongbao);
    }

    // all processes Nhan Ma Tran: Cs[ms][p] = As[ms][n] x B[n][p]
    for (int i = 0 ; i < ms; i++ )
        for (int k = 0 ; k < p ; k++ ) {    
            
            int sum = 0;
            for (int j = 0 ; j < n ; j++ )
                // *(C +  i*p + k) = *(C +  i*p + k) + (*(A + i*n + j)) * (*(B + j*p + k));
                sum += (*(As + i*n + j)) * (*(B + j*p + k));
            
            *(Cs +  i*p + k) = sum;
        }    

    
    // gui ket qua Cs cua process 1 va 2 ve process 0
    if (processId != 0) {
        MPI_Send(Cs, ms * p, MPI_INT, 0, processId+200, MPI_COMM_WORLD);
    }

    // process 0 nhan ket qua Cs tu nhieu process
    if (processId == 0) {
        // save Cs of process 0 into C first
        for (int i = 0 ; i < ms ; i++ )
            for (int j = 0 ; j < p ; j++ ){
                C[i*p + j] = Cs[i*p + j];
            }

        // receive Cs of process 1 and 2 and save into C in process 0
        for (int cpu = 1; cpu < num_process; cpu++) {
            MPI_Recv(C + ((cpu * ms) * p), ms * p, MPI_INT, cpu, cpu+200, MPI_COMM_WORLD, &thongbao);
        }

        // in ra
        printf( "Ma tran C:\n");
        DisplayMatrix(C, m, p);
    }

    MPI_Finalize();
    return 0;
}


// MPI_Send( const void* buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm);
// MPI_Recv( void* buf , int count , MPI_Datatype datatype , int source , int tag , MPI_Comm comm , MPI_Status* status);