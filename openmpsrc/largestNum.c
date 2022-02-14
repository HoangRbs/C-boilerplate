#include <stdio.h>
#include <omp.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// 1,2,3,4,5,6,7,8,9,10,11 
// bước 1: chia đều
// 5(thread) , size 11 --> 11 / 5 = 2 du 1
// 1,2   3,4   5,6   7,8   9,10   dư 1 số (11)
// bước 2: bỏ số dư
// 1,2,3(0)    4,5(1)     6,7(2)     8,9(3)     10,11(4)  dư ()

// buoc 1: chia đều
// Muốn 6(thread) ==> 11 / 6 = 1 du 5  
// 1,   2    3    4    5   6,  dư số (7,8,9,10,11) 
// buoc 2: bỏ số dư
// lấy số dư (5) thêm dần vào các thread còn lại
// VD: 
// 1,   2,11   3,10   4,9   5,8   6,7   dư()
 
// qua 2 vd trên: + thêm TH: số dư luôn bé hơn số chia

// ta lấy ra một thuật toán chung:
// mỗi thread thì sẽ có một số lượng phần tử riêng
// xét VD2 có 6 thread tức mảng sau có 6 phần tử
// mảng elementsPerThread [   ,   ,   ,   ,   ,   ]; 
// bước 1: chia đều, thì từng phần tử = 1, và số dư = 5 --> elemPerThread [1,1,1,1,1,1] dư = 5;
// bước 2: loại bỏ dần số dư, mỗi lần loại bỏ thì tăng từng phần tử lên 1 đơn vị
// ---> for i = 0 tới (dư = 5 - 1) --> [1,1,1,1,1,1] ---> [2,2,2,2,2,1]

// tính startIndex:
// ở VD 1: startIndex tại thread 0 = 0
// startIndex tại thread 1 = 3 == elems của thread 0
// startIndex tại thread 2 = 5 == elems của thread 0 + elems của thread 1
// startIndex tại thread i = tổng elems của các thread trước đó từ 0 tới i - 1;
// lastIndex thread i =  startIndex thread i + elems thread i - 1;

int main() {
    /* initialize random seed: */
    srand (time(NULL));

    int arrSize = 11;
    int *intArr = malloc(arrSize * sizeof(int));

    for (int i =0; i < arrSize; i++) {
        // printf("nhap vao IntArr[%d]: ", i);
        // scanf("%d", &intArr[i]);
        // printf("\n");

        intArr[i] = rand() % 20;
    }

    for (int i =0; i < arrSize; i++) {
        printf("%d ", intArr[i]);
    }

    printf("\n");

    // ------ setup thread, maxNumPerThread, .... thread related variable -------
    const int numThreads = 6;
    omp_set_num_threads(numThreads);
    int *elemsPerThread = malloc(numThreads * sizeof(int));         // elements per thread

    int maxNumPerThread = 0;
    maxNumPerThread = arrSize / numThreads;

    // bước 1:
    for(int i = 0; i < numThreads;i++) {
        elemsPerThread[i] =  maxNumPerThread;
    }

    // bước 2:
    int so_du = arrSize % numThreads;
    for(int i = 0; i < so_du; i++) {
        elemsPerThread[i] += 1;
    }

    // buoc 3:
    int *startIndexPerThread =  malloc(numThreads * sizeof(int));
    int *lastIndexPerThread =  malloc(numThreads * sizeof(int));

    for(int i = 0; i < numThreads;i++) {
        startIndexPerThread[i] = 0; // init
        lastIndexPerThread[i] = 0;

        if (i == 0) {
            startIndexPerThread[i] = 0; 
        } else {
            // start calculate start index for current thread i
            for (int j = 0; j < i; j++) {
                startIndexPerThread[i] += elemsPerThread[j];
            }
        }

        // cal last index for current thread i
        lastIndexPerThread[i] = startIndexPerThread[i] + elemsPerThread[i] - 1; 
    }

    // --- global var ---------
    int globalLargestNumber = intArr[0];

    // ----------- thread scope var ------------- datas work independently from each other
    int largestNumber = 0;
    int i = 0;

    // parallel
    #pragma omp parallel private(largestNumber, i)
    {
        // int totalThread = omp_get_num_threads();
        int threadID = omp_get_thread_num();

        // calculate start index and end index inside array for eachThread to handle
        int startIndex = startIndexPerThread[threadID];
        int lastIndex = lastIndexPerThread[threadID];
        
        largestNumber = intArr[startIndex];

        for (i = startIndex; i <= lastIndex; i++) {
            if (intArr[i] > largestNumber) largestNumber = intArr[i];
        }

        if (largestNumber > globalLargestNumber) globalLargestNumber = largestNumber;
    }

    printf("largestNumber = %d \n", globalLargestNumber);
    return 0;
}