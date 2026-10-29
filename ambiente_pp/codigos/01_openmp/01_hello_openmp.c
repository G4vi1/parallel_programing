/* OpenMP 01 — Regiao paralela e identificacao de threads
 * Compilar: gcc -O2 -fopenmp -o 01_hello_openmp 01_hello_openmp.c
 * Executar: OMP_NUM_THREADS=4 ./01_hello_openmp
 */
#include <stdio.h>
#include <omp.h>

int main(void) {
    printf("Threads disponiveis: %d\n", 
        /*Pega o máximo de threads disponíveis no computador
        */
        omp_get_max_threads()
    );
    #pragma omp parallel
    {
        /* opm_get_thread_num(); > identifica quem é a thread atual
           opm_get_num_threads(); > informa quantas threads no total 
           existem naquela região paralela
        */
        int id = omp_get_thread_num();
        int n  = omp_get_num_threads();
        printf("  Ola da thread %d de %d\n", id, n);
    }
    return 0;
}
