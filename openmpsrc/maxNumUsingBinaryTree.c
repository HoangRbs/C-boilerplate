// #include <iostream>
#include <stdio.h>
#include <omp.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <math.h>

// using namespace std;

// bool isEven (int num) {
//     if (num % 2 == 0) return true;
//     return false;
// }

// int roundUp (float num) {   // 9 / 2 == 4.5 --> 5 but not used for 8 / 2 = 4 --> 4
//     return floor(num) + 1;
// }

// int* deepCopyArray (int* arr, int arrSize) {
//     int* newArr = new int[arrSize];
//     for (int i = 0; i < arrSize; i++) {
//         newArr[i] = arr[i];
//     }   
//     return newArr;
// }

int main() {
    srand (time(NULL));

    /* search max number using binary tree and threads */
    int arrSize = 8;    // the size should be even
    int* m_arr = malloc(arrSize * sizeof(int));

    for (int i = 0; i < arrSize; i++) {
        m_arr[i] = rand() % 10 + 1;
        // cout << m_arr[i] << " "; 
        printf("%d ", m_arr[i]);
    }    
    // cout << endl;
    printf("\n");
    
    int currentNumThreads =  arrSize / 2;
    int level = 0;      // binary tree level

    while (1) {
        omp_set_num_threads(currentNumThreads);             // set numbers of threads

        // ----------- thread scope variables ------------- // 
        int leftIndex = 0;
        int rightIndex = 0;
        int threadId = 0;

        #pragma omp parallel private(leftIndex, rightIndex, threadId)
        {       
            threadId = omp_get_thread_num();
            leftIndex = threadId * pow(2, level + 1);
            rightIndex = leftIndex + pow(2, level);
            m_arr[leftIndex] = max(m_arr[leftIndex], m_arr[rightIndex]);
        }

        if (currentNumThreads == 1) {           // the final result is in m_arr[0] and the amount of threads === only 1 thread left
            // cout << "max: " << m_arr[0] << endl;
            printf("max: %d \n", m_arr[0]);
            break;   
        }

        // setting for the next round
        currentNumThreads /= 2;
        level++;
    }
}