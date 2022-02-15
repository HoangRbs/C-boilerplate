// mpicc file.c
// run ./a.out no need to specify number of thread

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define M 9
#define Ntime 1000
#define dt 0.01
#define dx 0.1
#define D 0.1

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

int main() {

    float *T, *dT;  // nhiet do thanh kim loai
    T = (float*) malloc ((M) * sizeof(float));
    dT = (float*) malloc ((M) * sizeof(float));
    KhoiTao(T);


    for (int t = 0; t < Ntime; t++) {       // xet nhiet do thanh kim loai tung thoi diem t.
        DHB2(T, dT); // input thanh kim loai T, ouput dT

        // tinh lai T : input dT
        for (int i = 0; i < M; i++) {
            *(T+i) = *(T + i) + dt * (*(dT + i));
        }
    }

    // in ra nhiet do tren thanh kim loai
    for (int i = 0; i < M; i++) {
        printf("%d: %f \n", i, *(T+i));
    }
    return 0;
}