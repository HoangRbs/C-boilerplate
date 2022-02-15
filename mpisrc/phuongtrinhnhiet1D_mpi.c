// mpicc file.c
// mpirun -np 3 ./a.out 

#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>

#define M 9
#define Ntime 1000
#define dt 0.01
#define dx 0.1
#define D 0.1

void KhoiTaoNhietDo(float *T) {
    // thank kim loai 25 do c
    for (int i = 0; i < M; i++) {
        *(T + i) = 25.0;
    }
}

void DHB2 (float *T_cpu, float *dT, float tl, float tr , int ms) {
    float c, l, r;
    for (int i = 0; i < ms; i++) {
        c = *(T_cpu + i);
        l = (i == 0) ? tl : *(T_cpu + (i - 1));
        r = (i == ms - 1) ? tr : * (T_cpu + (i + 1));
        *(dT + i) = D*(r - 2*c + l)/(dx*dx);
    }
}

void sendTlTr(float* T_cpu, float* tl, float* tr, int processId, int numberProcess, MPI_Status status, int ms);

int main(int argc, char *argv[]) {
    
    MPI_Init(&argc, &argv);                         // must have argument so the command from terminal can pass arguments into program
    
    int num_process, processId;
    MPI_Status status;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &processId);      // get OUT id_process   

    float *T, *dT;
    T = (float*) malloc ((M) * sizeof(float));  // thanh kim loai
    dT = (float*) malloc ((M) * sizeof(float));

    if (processId == 0) 
        KhoiTaoNhietDo(T);     // khoi tao nhiet do thanh kim loai

    // chia nho cho cac process
    int ms = M / num_process; // M = 9; num Processes = 3 --> ms = 3

    float *T_cpu = (float*) malloc ((ms) * sizeof(float)); // chia nhỏ thanh kim loại cho từng cpu.
    float tr, tl;

    // scatter T thanh cac T_cpu
    MPI_Scatter(T, ms, MPI_FLOAT , T_cpu, ms, MPI_FLOAT, 0, MPI_COMM_WORLD);

    for (int t = 0; t < Ntime; t++) {       // xet nhiet do thanh kim loai tung thoi diem t.
        tr = 25.0;  // thanh kim loai different each time so tr, tl are.
        tl = 25.0;  // reset to 25 degree

        sendTlTr(T_cpu, &tl, &tr, processId, num_process, status, ms);

        DHB2(T_cpu, dT, tl, tr, ms); // input thanh kl T_cpu, ouput dT

        // tinh lai T_cpu : input dT
        for (int i = 0; i < ms; i++) {
            *(T_cpu + i) = *(T_cpu + i) + dt * (*(dT + i));
        }
    }
    
    // gather cac T_cpu vao T
    MPI_Gather( T_cpu, ms, MPI_FLOAT , T , ms , MPI_FLOAT , 0 , MPI_COMM_WORLD);

    if (processId == 0)
        // in ra nhiet do tren thanh kim loai
        for (int i = 0; i < M; i++) {
            printf("%d: %f \n", i, *(T+i));
        }


    MPI_Finalize();

    return 0;
}


void sendTlTr(float* T_cpu, float* tl, float* tr, int processId, int numberProcess, MPI_Status status, int ms) {

    float sendTl;
    float sendTr;

    // tl
    if(processId == 0) {

        // periodic boudary: tl of process 0 is 100 degree
        *tl = 100.0; 
        // send tl to next process
        sendTl = T_cpu[ms - 1];

        // send tl to next process
        // send and recv tag must be equal: cpu 0 send with tag 0 then cpu 1 recv also with tag 0.
        MPI_Send(&sendTl, 1, MPI_FLOAT, processId + 1, processId, MPI_COMM_WORLD);


    }
    else if(processId == numberProcess - 1) {
        // recv tl data from prev process
        MPI_Recv(tl, 1, MPI_FLOAT, processId - 1, processId - 1, MPI_COMM_WORLD, &status);
    }
    else {
        // send tl to next process
        sendTl = T_cpu[ms - 1];
        MPI_Send(&sendTl, 1, MPI_FLOAT, processId + 1, processId, MPI_COMM_WORLD);

        // recv tl data from prev process
        MPI_Recv(tl, 1, MPI_FLOAT, processId - 1, processId - 1, MPI_COMM_WORLD, &status);
    }

    // tr
    if(processId == numberProcess - 1) {

        // periodic boudary: tr of last process  is 25 degree
        *tr = 25.0; 
        // send tr to prev process
        sendTr = T_cpu[0];
        MPI_Send(&sendTr, 1, MPI_FLOAT, processId - 1, processId, MPI_COMM_WORLD);
    }
    else if(processId == 0) {
        // recv tr data from next process
        MPI_Recv(tr, 1, MPI_FLOAT, processId + 1, processId + 1, MPI_COMM_WORLD, &status);
    }
    else {

        // send tr to prev process
        sendTr = T_cpu[0];
        MPI_Send(&sendTr, 1, MPI_FLOAT, processId - 1, processId, MPI_COMM_WORLD);
        
        // recv tr data from next process
        MPI_Recv(tr, 1, MPI_FLOAT, processId + 1, processId + 1, MPI_COMM_WORLD, &status);
    }
}