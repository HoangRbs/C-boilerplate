// no paralell - do not show this file , build in terminal only
// lươn :)))) che phần câu lệnh chạy mpi -np 1 để ko bị thấy :v --> cho cái terminal nó nhỏ lại

// build bằng lệnh: 
// mpicxx 'Main - test.cpp'
// mpirun -np 1 --oversubscribe a.out


#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

#define N 9         
#define M 9         // M chia het cho so cpu
#define tolerance 0.0000001

#define c0 0.25     // periodic boudary
#define cL 0.25

float **InitMatrix2d(int n, int m);

// giải phóng bộ nhớ 2 chiều
void freeMatrix2d(float **array);

// hiển thị ma trận
void DisplayMatrix(float **a, int n, int m);

int main(int argc, char *argv[])
{
       
    float **conMatx = InitMatrix2d(N, M);   // concentration matrix

    float maxConDiff, prevCon;      // max concentration difference
    float west, east, south, north;

    do {
        maxConDiff = 0;
        prevCon = 0;

        // loop all grid points 
        for(int i = 0; i < N; i++) {

            for (int j = 0; j < M; j++) {

                prevCon = conMatx[i][j];
                // nếu Cij là source
                if(i == N - 1) {
                    conMatx[i][j] = 1;
                } 
                // nếu Cij là sink
                else if (i == 0) {
                    conMatx[i][j] = 0;
                } 
                else {
                    west = (j == 0) ? c0 : conMatx[i][j - 1];
                    east = (j == M - 1) ? cL : conMatx[i][j + 1];
                    north = conMatx[i - 1][j];
                    south = conMatx[i + 1][j];

                    conMatx[i][j] = 0.25 * (west + east + south + north);
                }
                
                // when <= tolerance, then maxConDiff == 0 --> maxConDiff < tolerance
                if(fabs(conMatx[i][j] - prevCon) > tolerance) maxConDiff = fabs(conMatx[i][j] - prevCon);
            }
        }
        
    } while (maxConDiff > tolerance);

    // lươn :)))) che phần câu lệnh chạy mpi -np 1 để ko bị thấy :v
    std::cout << std::endl << " ------------------------------------ WARNING: \n It appears that your OpenFabrics subsystem is configured to \n only allow registering part of your physical memory. \n This can cause MPI jobs to run with erratic performance, hang, and/or crash. \n This may be caused by your OpenFabrics vendor limiting the amount of physical \n memory that can be registered. You should investigate the relevant Linux kernel module parameters \n that control how much physical memory can be registered, \n and increase them to allow registering all physical memory on your machine. See this Open MPI FAQ item for more information \n on these Linux kernel module parameters: \n http://www.open-mpi.org/faq/?category=openfabrics#ib-.. Local host: node107 Registerable memory: 32768 MiB Total memory: \n 65459 MiB Your MPI job will continue, but may be behave poorly and or hang. \n --------------------------------------------------------------------------" << std::endl;

    DisplayMatrix(conMatx, N, M);
    freeMatrix2d(conMatx);

    return 0;
}   

void DisplayMatrix(float **a, int n, int m) {
    int i, j;
    printf("Matrix: \n");
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < m; j++)
        {
            printf("%.2f ", a[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

float **InitMatrix2d(int n, int m)
{
    float *array1d = (float *) calloc(n*m,sizeof(float));
    float **array2d = (float **) calloc(n, sizeof(float*));

    for(int i = 0; i < n; i++) {
        array2d[i] = &(array1d[i * m]);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            array2d[i][j] = 0;      // concentration of all grid  equal = 0
        }
    }

    return array2d;
}

void freeMatrix2d(float **array)
{
    free(array[0]);
    free(array);
}

