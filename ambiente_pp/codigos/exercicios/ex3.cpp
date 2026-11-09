#include <omp.h>
#include <iostream>

int main() {

    /*
    Este programa mostra como descobrir o número de processadores lógicos
    disponíveis e o número máximo de threads que o OpenMP usaria por padrão.
    */
    int num_procs = omp_get_num_procs();
    int max_threads = omp_get_max_threads();

    std::cout << "Processadores lógicos: " << num_procs << '\n';
    std::cout << "Máximo de threads: " << max_threads << '\n';


    /*Aqui ele forma uma equipe de threads*/
    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int total = omp_get_num_threads();


        /*
        Aqui ele divide o for entre as threads, 
        cada thread executa uma parte do for.
        */
        #pragma omp for
        for (int i = 0; i < 8; ++i) {

            /* 
                A diretiva critical garante que apenas uma thread por vez
                executará o bloco de código dentro dela, evitando que múltiplas threads
                imprimam ao mesmo tempo e causem confusão na saída.
            */
            #pragma omp critical
            std::cout << "Thread " << tid
                      << " de " << total
                      << ", i = " << i << '\n';
        }
    }

    return 0;
}

/*
Processadores lógicos: 16
Máximo de threads: 16
Thread 1 de 16, i = 1
Thread 7 de 16, i = 7
Thread 6 de 16, i = 6
Thread 3 de 16, i = 3
Thread 2 de 16, i = 2
Thread 5 de 16, i = 5
Thread 4 de 16, i = 4
Thread 0 de 16, i = 0

*/