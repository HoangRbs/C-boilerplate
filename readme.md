----------- for project ------------------------------------

0. for building and running "src" code (can contains openmp code)
1. install g++ on linux
2. in terminal run:
3. cmake . to build project stucture
4. make to build into executable file "HelloWorld"
5. run ./HelloWorld







--------------------------- MPI ------------------------------
6. for building and running mpi source code "mpisrc"
7. install mpi
8. mpicxx file.cpp (build)
9. mpirun -np 5 ./a.out (run), np là number of processor

----------------------------- OPEN MP ------------------------------

6. for building and running mpi source code "openmp src"
7. install openmp
8. gcc -fopenmp hello.c (build) 
9. ./a.out (run)
