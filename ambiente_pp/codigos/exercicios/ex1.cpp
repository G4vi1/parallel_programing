#include <cstdio>
#include <omp.h>

int main() {
    printf("Antes da região paralela\n");

    /* pragma é diretiva do compilador. 
    omp é a abreviação de OpenMP, que é uma API para programação paralela em C/C++ e Fortran.
    parallel indica que o bloco de código a seguir será executado em paralelo por múltiplas
    num_threads(4) especifica que queremos usar 4 threads para executar o bloco de código em paralelo.
    
    */

    #pragma omp parallel num_threads(4)
    {
        /*
        omp_get_thread_num() retorna o identificador da thread atual (um número inteiro único para cada thread).
        omp_get_num_threads() retorna o número total de threads que estão executando o bloco de
        */
        int id = omp_get_thread_num();
        int total = omp_get_num_threads();

        printf("Olá da thread %d de %d\n", id, total);
    }

    printf("Depois da região paralela\n");
    return 0;
}

/*Resultado:

Antes da região paralela
Olá da thread 1 de 4
Olá da thread 3 de 4
Olá da thread 0 de 4
Olá da thread 2 de 4
Depois da região paralela

Termina desordenado pois as 4 threads estão executando ao mesmo tempo
e o openmp não garante a ordem de execução das threads.