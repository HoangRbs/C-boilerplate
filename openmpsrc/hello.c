#include <stdio.h>
#include <omp.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

int main() {
    int id, x;
    omp_set_num_threads(100);

    // song song từng thread
    #pragma omp parallel private(id, x) 
    {
        int totalThread = omp_get_num_threads();
        int threadID = omp_get_thread_num();

        id = omp_get_thread_num();
        x = 10 * id;
        printf("\n");
        printf("Hellow from thread id = %d, x = %d, in total of %d", id, x, totalThread);
        printf("\n");
    }
}