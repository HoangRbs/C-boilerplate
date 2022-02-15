// #include <stdio.h>
// #include <mpi.h>
// #include <stdlib.h>
// #include <math.h>
// #include <time.h>

// // mpicc file.cpp
// // mpirun -np 3 ./a.out 

// int main(int argc, char *argv[]) {
//     srand (time(NULL));
    
//     MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
//     int num_process, processId;

//     MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
//     MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   
//     MPI_Status thongbao;


//     printf("hello from process %d trong tat ca %d cpu \n", processId, num_process);

//     MPI_Finalize();
//     return 0;
// }

// Bai Toan Nhan 2 Ma tran: A[m][n] x B[n][p] = C[m][p]
#include <stdio.h>
#include <malloc.h>

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


int main(int argc, char *argv[])
{
    int i, j, k;

    // Khai bao ma tran A, B, C
    int *A, *B, *C;
    A = (int *) malloc ((m*n)*sizeof(int));
    B = (int *) malloc ((n*p)*sizeof(int));
    C = (int *) malloc ((m*p)*sizeof(int));

    // Khoi Tao Du Lieu (Ma tran A, B)
    //Ma tran A la ma tran Don vi
    for (  i = 0 ; i < m ; i++ )
        for ( j = 0 ; j < n ; j++ ){
            if (i==j) 
                *(A+i*n+j) = 1.;
            else
                *(A+i*n+j) = 0;
        }

    printf("The matrix A:\n");
    DisplayMatrix(A, m, n);
    
    //Ma tran B
    for (  i = 0 ; i < n ; i++ )
        for ( j = 0 ; j < p ; j++ ){
            *(B+i*p+j) = 1*(i*p+j);
        }
    
    printf("The matrix B\n");
    DisplayMatrix(B, n, p);

    // Nhan Ma Tran: C[m][p] = A[m][n] x B[n][p]
    for ( i = 0 ; i < m ; i++ )
        for ( k = 0 ; k < p ; k++ ) {    
            
            int sum = 0;
            for ( j = 0 ; j < n ; j++ )
                // *(C +  i*p + k) = *(C +  i*p + k) + (*(A + i*n + j)) * (*(B + j*p + k));
                sum += (*(A + i*n + j)) * (*(B + j*p + k));
            
            *(C +  i*p + k) = sum;
        }
            
    
    printf( "Ma tran C:\n");
    DisplayMatrix(C, m, p);

    return 0;
}
