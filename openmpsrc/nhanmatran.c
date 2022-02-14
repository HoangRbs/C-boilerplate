#include <stdio.h>
#include <omp.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// code mới sử dụng ý tưởng chia đều 3 thread cho B và C nếu là ma trận 3 x 3
// thì mỗi thread 1 cột () hay 6 x 6 thì có 6 thread --> ko tối ưu

// nhưng nếu là ma trậN 6 x 6 hay chia đều cho 2 thread (chỉ sử dụng th chia hết)
// --> thì mỗi thread là 3 cột
// hay 9 x 9 chia đều cho 3 thread --> mối thread 3 cột
// 10 x 10 --> 2 thread : mỗi thread 5 cột
// 10 x 10 --> 5 thread : mỗi thread 2 cột
// --> mai 6h làm

int main() {

    /*  nhan ma tran */
    /* initialize random seed: */
    srand (time(NULL));

    const int matrixSize = 4;

    // new int [matrixSize * matrixSize];
    // new int [matrixSize * matrixSize];
    // new int [matrixSize * matrixSize];

    int *matrixA = malloc(matrixSize * matrixSize * sizeof(int));
    int *matrixB = malloc(matrixSize * matrixSize * sizeof(int));
    int *matrixC = malloc(matrixSize * matrixSize * sizeof(int));

    // std::cout<< "------------ maTrixA: ---------" << std::endl;
    printf("------------ maTrixA: --------- \n");
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matrixA[i * matrixSize + j] = rand() % 4 + 1;
            // std::cout << matrixA[i * matrixSize + j] <<" "; 
            printf("%d ", matrixA[i * matrixSize + j]);
        }
        // std::cout << std::endl;
        printf("\n");
    }

    // std::cout<< "------------ maTrixB: ---------" << std::endl;
    printf("------------ maTrixB: --------- \n");
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matrixB[i * matrixSize + j] = rand() % 4 + 1;
            // std::cout << matrixB[i * matrixSize + j] <<" ";
            printf("%d ", matrixB[i * matrixSize + j]);
        }
        // std::cout << std::endl;
        printf("\n");
    }

    int numThreads = 2;
    int columnsPerThread = matrixSize / numThreads; // chọn số để chia hết, matrixSize = 4, thread = 2 --> colPerThread = 2


    omp_set_num_threads(numThreads); // set numbers of threads

    // ----------- thread scope variables ------------- // 
    int i = 0;
    int j = 0;  // threadId
    int Aj = 0; 
    int sum = 0;

    #pragma omp parallel private(i, j, Aj, sum)
    {
        sum = 0; // must reinitialize
        j = omp_get_thread_num();   // j is threadId

        int hangA = matrixSize;
        int cotA = matrixSize;

        for (i = 0; i < cotA; i++) {
            
            for (int currentCol = 0; currentCol < columnsPerThread; currentCol++) {
                int Bi; // for B matrix
                int Bj;

                for (Aj = 0; Aj < hangA; Aj++) {
                    Bi = Aj; // for now Bi == Aj since we're just using square matrix
                    Bj = j * columnsPerThread + currentCol;

                    sum += matrixA[i * matrixSize + Aj] * matrixB[Bi * matrixSize + Bj];
                }

                matrixC [i * matrixSize + Bj] = sum;
                sum = 0;
            }
        }
    }

    // std::cout<< "------------ maTrixC: ---------" << std::endl;
    printf("------------ maTrixC: --------- \n");
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            // std::cout << matrixC[i * matrixSize + j] <<" ";
            printf("%d ", matrixC[i * matrixSize + j]);
        }
        // std::cout << std::endl;
        printf("\n");
    }


    return 0;
}