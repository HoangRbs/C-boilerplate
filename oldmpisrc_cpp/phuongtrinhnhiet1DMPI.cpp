#include <stdio.h>
#include <malloc.h>
#include <mpi.h>

#define M 10
#define Ntime 1000
#define dt 0.01
#define dx 0.1
#define D 0.1

// làm tiếp dùng mpi khi truyền thông, sử dụng gather , scatter , các thứ

void KhoiTao(float *T) {
    // thank kim loai 25 do c
    for (int i = 0; i < M; i++) {
        *(T + i) = 25.0;
    }
}

void DHB2 (float *T, float *dT) {
    float c, l, r;
    for (int i = 0; i < M; i++) {
        c = *(T + i);
        l = (i == 0) ? 100.0 : *(T + (i - 1));
        r = (i == M - 1) ? 25.0 : * (T+(i + 1));
        *(dT + i) = D*(r - 2*c + l)/(dx*dx);
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv); // must have argument so the command from terminal can pass arguments into program
    
    int num_process, id_process;
    MPI_Status thongbao;

    MPI_Comm_size(MPI_COMM_WORLD, &num_process);    // get OUT num_processes
    MPI_Comm_rank(MPI_COMM_WORLD, &id_process);     // get OUT id_process

    // -------------------------------------------------------

    float *T, *dT;
    T = (float*) malloc ((M) * sizeof(float));
    dT = (float*) malloc ((M) * sizeof(float));
    KhoiTao(T);
    for (int t = 0; t < Ntime; t++) {
        DHB2(T, dT); // input T, ouput dT

        // tinh lai T : input dT
        for (int i = 0; i < M; i++) {
            *(T+i) = *(T + i) + dt * (*(dT + i));
        }
    }

    // in ra nhiet do tren thanh kim loai
    for (int i = 0; i < M; i++) {
        printf("%d: %f \n", i, *(T+i));
    }

    // --------------------------------------------------------
    
    MPI_Finalize();
    return 0;
}