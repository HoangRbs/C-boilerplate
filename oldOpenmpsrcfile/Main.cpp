#include <iostream>
#include <omp.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main() {
    // int id, x;

    // omp_set_num_threads(100);
    // #pragma omp parallel private(id, x) 
    // {
    //     // int threadnumber = omp_get_num_threads();
    //     // int threadID = omp_get_thread_num();

    //     // std::cout << "Hello from thread " << threadID << " nthreads " << threadnumber << std::endl;

    //     // id = omp_get_thread_num();
    //     // x = 10 * id;
    //     // printf("\n");
    //     // printf("Hellow from thread id = %d, x = %d", id, x);
    //     // printf("\n");
    // }

    /* bai tap tim sum va max
    int arrSize = 11;
    int *IntArr = new int[arrSize];
    for (int i =0; i < arrSize; i++) {
        printf("nhap vao IntArr[%d]: ", i);
        std::cin >> IntArr[i];
        printf("\n");
    }

    for (int i =0; i < arrSize; i++) {
        std::cout << IntArr[i] << " ";
    }

    printf("\n");

    int globalSum = 0;
    int globalLargestNumber = IntArr[0];
    const int maxNumsPerThread = 4;  // each thread handles a number of elements

    // maxNumPerThread = 2
    // 1,2, 3,4, 5,6, 7,8, 9      (9 elements) ---> 9/2 + 1 = 5 threads
    // 1,2, 3,4, 5,6, 7,8, 9,10 11  (11 elements) ---> 11/2 + 1 = 6 threads
    // (10 elements) ---> 10/2 + 0 = 5 threads 

    // maxNumPerThread = 4
    // 1,2,3,4, 5,6,7,8, 9,10   (10 elements) ---> 10/4 + 1 = 3 threads

    if (arrSize % maxNumsPerThread == 0) omp_set_num_threads(arrSize / maxNumsPerThread);
    else omp_set_num_threads(arrSize / maxNumsPerThread + 1);


    // ----------- thread scope variables ------------- // datas work independently from each other
    int sum = 0;
    int startIndex = 0;
    int lastIndex = 0;
    int largestNumber = 0;
    int i = 0; 

    #pragma omp parallel private(startIndex, lastIndex, sum, largestNumber, i)
    {
        sum = 0;
        // cal start and end index inside array for eachThread to handle
        startIndex = omp_get_thread_num() * maxNumsPerThread;
        lastIndex = startIndex + maxNumsPerThread - 1;
        
        // handle the last thread which might has fewer elements than default
        if (lastIndex > arrSize - 1) lastIndex = arrSize - 1;
        
        largestNumber = IntArr[startIndex];

        for (i = startIndex; i <= lastIndex; i++) {
            sum += IntArr[i];
            if (IntArr[i] > largestNumber) largestNumber = IntArr[i];
        }

        globalSum += sum;
        if (largestNumber > globalLargestNumber) globalLargestNumber = largestNumber; 
    }

    printf("sum = %d and largestNumber = %d \n", globalSum, globalLargestNumber);
    */

    /*  nhan ma tran */
    /* initialize random seed: */
    srand (time(NULL));

    const int matrixSize = 3;

    int *matrixA = new int [matrixSize * matrixSize];
    int *matrixB = new int [matrixSize * matrixSize];
    int *matrixC = new int [matrixSize * matrixSize];

    std::cout<< "------------ maTrixA: ---------" << std::endl;
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matrixA[i * matrixSize + j] = rand() % 4 + 1;
            std::cout << matrixA[i * matrixSize + j] <<" ";
        }
        std::cout << std::endl;
    }

    std::cout<< "------------ maTrixB: ---------" << std::endl;
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            matrixB[i * matrixSize + j] = rand() % 4 + 1;
            std::cout << matrixB[i * matrixSize + j] <<" ";
        }
        std::cout << std::endl;
    }

    omp_set_num_threads(matrixSize); // set numbers of threads

    // ----------- thread scope variables ------------- // 
    int i = 0;
    int j = 0;  // threadId
    int Aj = 0; // for matrixA only
    int sum = 0;

    #pragma omp parallel private(i, j, Aj, sum)
    {
        sum = 0; // must reinitialize
        j = omp_get_thread_num();   // threadId
        
        for (i = 0; i < matrixSize; i++) {
            for (Aj = 0; Aj < matrixSize; Aj++) {
                sum += matrixA[i * matrixSize + Aj] * matrixB[Aj * matrixSize + j];
            }

            matrixC [i * matrixSize + j] = sum;
            sum = 0;
        }
    }

    std::cout<< "------------ maTrixC: ---------" << std::endl;
    for (int i = 0; i < matrixSize; i++) {
        for (int j = 0; j < matrixSize; j++) {
            std::cout << matrixC[i * matrixSize + j] <<" ";
        }
        std::cout << std::endl;
    }


    return 0;
}