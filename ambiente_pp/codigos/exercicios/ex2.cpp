#include <cstdio>
#include <omp.h>
#include <iostream>

int main(){

    /*
    Aqui a diretiva de pragma pediu 2 threads para ser armazenada na região paralela.+
    */
    #pragma omp parallel num_threads(2)
    {
        int id = omp_get_thread_num();

        /*
        omp_get_thread_num() retorna o identificador da thread atual 
        (um número inteiro único para cada thread).
        */

        for(int i = 0; i < 4; i++){
            printf("Thread %d: i = %d\n", id, i);
        }
    }
    return 0;
}

/*
resultado do pragma:

Thread 1: i = 0
Thread 1: i = 1
Thread 1: i = 2
Thread 1: i = 3
Thread 0: i = 0
Thread 0: i = 1
Thread 0: i = 2
Thread 0: i = 3

O parallel criou duas threads, mas não dividiu o for. 
Por isso, cada thread executou todas as iterações 
de 0 a 3, totalizando 8 execuções.
*/