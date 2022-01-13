#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>

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

// gửi cạnh trái phải cho các tiến trình
void sendTlTr(float **c, float ** tl, float **tr, int processId, int numberProcess, MPI_Status status, int ms);

int main(int argc, char *argv[])
{
    
    // MPI_Abort( MPI_COMM_WORLD , 005);
    
    MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
    int num_process, processId;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   

    // if (M % num_process != 0) {                     // M nen chia het cho so process
    //     printf("ko chia het cho so process!!");
    //     exit;
    // }

    // -------------------------------------------------------
    
    printf("starting mpi");

    float **conMatx = InitMatrix2d(N, M);   // concentration matrix
    float *sendptr, *recvptr;

    int ms = M / num_process;
    float **conMatx_cpu = InitMatrix2d(N, ms);

    // define column type : a unit is a columns contains N squares.
    MPI_Datatype latticecol, latticecoltype, mscol, mscoltype;

    if(processId == 0)
    {
        sendptr = &(conMatx[0][0]);

        MPI_Type_vector(N, 1, M, MPI_FLOAT, &latticecol);
        MPI_Type_commit(&latticecol);
        MPI_Type_create_resized(latticecol, 0, 1 * sizeof(float), &latticecoltype);
        MPI_Type_commit(&latticecoltype);
    }
    else
    {
        sendptr = NULL;
    }

    MPI_Type_vector(N, 1, ms, MPI_FLOAT, &mscol);
    MPI_Type_commit(&mscol);
    MPI_Type_create_resized(mscol, 0, 1 * sizeof(float), &mscoltype);
    MPI_Type_commit(&mscoltype);

    MPI_Scatter(sendptr, ms, latticecoltype, &conMatx_cpu[0][0], ms, mscoltype, 0, MPI_COMM_WORLD);

    float **tl, **tr, **result;
    float local_maxConDiff, global_maxConDiff, prevCon;      // max concentration difference
    float west, east, south, north;

    do {
        local_maxConDiff = 0;
        global_maxConDiff = 0;
        prevCon = 0;

        tl = InitMatrix2d(N, 1);
        tr = InitMatrix2d(N, 1);

        // exchange boundary strips with neighboring processors
        // trao đổi miền cho mỗi tiến trình (tl, tr)
        
        sendTlTr(conMatx_cpu,tl,tr,processId, num_process, status, ms);

        // loop all grid points 
        for(int i = 0; i < N; i++) {

            for (int j = 0; j < ms; j++) {

                prevCon = conMatx_cpu[i][j];
                // nếu Cij là source
                if(i == N - 1) {
                    conMatx_cpu[i][j] = 1;
                } 
                // nếu Cij là sink
                else if (i == 0) {
                    conMatx_cpu[i][j] = 0;
                } 
                else {
                    west = (j == 0) ? tl[i][0] : conMatx_cpu[i][j - 1];
                    east = (j == ms - 1) ? tr[i][0] : conMatx_cpu[i][j + 1];
                    north = conMatx_cpu[i - 1][j];
                    south = conMatx_cpu[i + 1][j];

                    conMatx_cpu[i][j] = 0.25 * (west + east + south + north);
                }
                
                // when <= tolerance, then maxConDiff == 0 --> maxConDiff < tolerance
                if(fabs(conMatx_cpu[i][j] - prevCon) > tolerance) local_maxConDiff = fabs(conMatx[i][j] - prevCon);
            }
        }

        freeMatrix2d(tl);
        freeMatrix2d(tr);

        MPI_Allreduce(&local_maxConDiff, &global_maxConDiff, 1, MPI_FLOAT, MPI_MAX, MPI_COMM_WORLD);
        
    } while (global_maxConDiff > tolerance);

    if(processId == 0)  {
        result = InitMatrix2d(N,M);         // mảng 2 chiều để tiện thao tác
        recvptr = &result[0][0];            // mảng 1 chiều thì chỉ đến mảng 2 chiều để dễ làm việc với mpi
    } else {
        recvptr = NULL;
    }

    MPI_Gather(conMatx_cpu, N * ms, mscoltype, recvptr, N * M, latticecoltype, 0, MPI_COMM_WORLD);

    if(processId == 0) {
        DisplayMatrix(result, N, M);

        freeMatrix2d(conMatx);
        freeMatrix2d(result);
    }

    freeMatrix2d(conMatx_cpu);

    // --------------------------------------------------------
    MPI_Finalize();

    return 0;
}   

void sendTlTr(float **conMatx_cpu, float ** tl, float **tr, int processId, int numberProcess, MPI_Status status, int ms) {

    float** sendTl = InitMatrix2d(N, 1);
    float** sendTr = InitMatrix2d(N, 1);

    // tl
    if(processId == 0) {
        for (int i = 0; i < N; i++) {
            // periodic boudary: tl array of process 0 is [c0, c0, c0]
            tl[i][0] = c0; 
            // send tl to next process
            sendTl[i][0] = conMatx_cpu[i][ms - 1];
        }

        // send tl to next process
        // send and recv tag must be equal: cpu 0 send with tag 0 then cpu 1 recv also with tag 0.
        MPI_Send(sendTl, N, MPI_FLOAT, processId + 1, processId, MPI_COMM_WORLD);
    }
    else if(processId == numberProcess - 1) {
        // recv tl data from prev process
        MPI_Recv(&tl, N, MPI_FLOAT, processId - 1, processId - 1, MPI_COMM_WORLD, &status);
    }
    else {
        for (int i = 0; i < N; i++) {
            // send tl to next process
            sendTl[i][0] = conMatx_cpu[i][ms - 1];
        }

        // send tl to next process
        MPI_Send(sendTl, N, MPI_FLOAT, processId + 1, processId, MPI_COMM_WORLD);
        // recv tl data from prev process
        MPI_Recv(&tl, N, MPI_FLOAT, processId - 1, processId - 1, MPI_COMM_WORLD, &status);
    }

    // tr
    if(processId == numberProcess - 1) {
        for (int i = 0; i < N; i++) {
            // periodic boudary: tr array of process 0 is [cL, cL, cL]
            tr[i][0] = cL; 
            // send tr to prev process
            sendTr[i][0] = conMatx_cpu[i][ms - 1];
        }

        // send tl to prev process
        MPI_Send(sendTr, N, MPI_FLOAT, processId - 1, processId, MPI_COMM_WORLD);
    }
    else if(processId == 0) {
        // recv tr data from next process
        MPI_Recv(&tr, N, MPI_FLOAT, processId + 1, processId + 1, MPI_COMM_WORLD, &status);
    }
    else {
        for (int i = 0; i < N; i++) {
            // send tr to prev process
            sendTr[i][0] = conMatx_cpu[i][ms - 1];
        }

        // send tr to prev process
        MPI_Send(sendTr, N, MPI_FLOAT, processId - 1, processId, MPI_COMM_WORLD);
        // recv tr data from next process
        MPI_Recv(&tl, N, MPI_FLOAT, processId + 1, processId + 1, MPI_COMM_WORLD, &status);
    }
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

