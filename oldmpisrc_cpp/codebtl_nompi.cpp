// no paralell - do not show this file , build in terminal only

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

