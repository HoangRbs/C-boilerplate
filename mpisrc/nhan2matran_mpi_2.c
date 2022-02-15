#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// mpicc file.cpp
// mpirun -np 3 ./a.out 

// Nhap cac kich thuoc cua ma tran
#define  m       6
#define  n       6
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

    // send elements A from 0 to processes 0,1,2 to init their own As
    // MPI_Scatter( const void* sendbuf , int sendcount , MPI_Datatype sendtype , void* recvbuf , int recvcount , MPI_Datatype recvtype , int root , MPI_Comm comm);
    MPI_Scatter(A, ms * n, MPI_INT, As, ms * n, MPI_INT, 0, MPI_COMM_WORLD); 
    
    // send B
    // MPI_Bcast(void* buffer , int count , MPI_Datatype datatype , int root , MPI_Comm comm);
    MPI_Bcast(B, n*p, MPI_INT, 0, MPI_COMM_WORLD);

    // all processes Nhan Ma Tran: Cs[ms][p] = As[ms][n] x B[n][p]
    for (int i = 0 ; i < ms; i++ )
        for (int k = 0 ; k < p ; k++ ) {    
            
            int sum = 0;
            for (int j = 0 ; j < n ; j++ )
                // *(C +  i*p + k) = *(C +  i*p + k) + (*(A + i*n + j)) * (*(B + j*p + k));
                sum += (*(As + i*n + j)) * (*(B + j*p + k));
            
            *(Cs +  i*p + k) = sum;
        }    

    // all process send Cs to process0 and merge into C
    MPI_Gather(Cs, ms * p, MPI_INT, C, ms * p, MPI_INT, 0, MPI_COMM_WORLD);

    if (processId == 0) {
        // in ra
        printf( "Ma tran C:\n");
        DisplayMatrix(C, m, p);
    }

    MPI_Finalize();
    return 0;
}


// MPI_Send( const void* buf , int count , MPI_Datatype datatype , int dest , int tag , MPI_Comm comm);
// MPI_Recv( void* buf , int count , MPI_Datatype datatype , int source , int tag , MPI_Comm comm , MPI_Status* status);
// MPI_Bcast(void* buffer , int count , MPI_Datatype datatype , int root , MPI_Comm comm);
/*  
broad cast will send copies of buffer to all processes 
except the root (we set as 0) and will not divide like scatter
*/