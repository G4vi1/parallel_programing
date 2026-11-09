#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int N = 10;
    std::vector<int> x(N);
    std::vector<int> y(N);
    std::vector<int> resultado(N);

    // Inicializa os vetores x e y com alguns valores

    for (int i = 0; i < N; ++i) {
        x[i] = i;
        y[i] = i + 1;
    }

    /*
    Aqui, a diretiva `#pragma omp parallel for` indica 
    que o loop `for` será executado em paralelo.

    Todavia a clausula schedule(static) indica que a distribuição 
    das iterações do loop entre as threads será feita de forma estática, 
    ou seja, cada thread receberá um bloco contíguo 
    de iterações para executar.
    */
    #pragma omp parallel for schedule(static)
    for (int i = 0; i < N; ++i) {
        resultado[i] = x[i] + y[i];
    }

    /*
        Houve a divisão correta do for entre as threads
        cada thread executou uma parte do for.
        pois o uso da clausula schedule(static) permitiu que o OpenMP dividisse 
        as iterações do loop de forma estática entre as threads disponíveis.

        2/10 = 5

        5 para 2 threads

        Thread 0: i = 0 até 4
        Thread 1: i = 5 até 9
    */

    // Imprime os resultados

    for (int i = 0; i < N; ++i) {
        std::cout << x[i] << " + " << y[i]
                  << " = " << resultado[i] << '\n';
    }

    return 0;
}

/*
0 + 1 = 1
1 + 2 = 3
2 + 3 = 5
3 + 4 = 7
4 + 5 = 9
5 + 6 = 11
6 + 7 = 13
7 + 8 = 15
8 + 9 = 17
9 + 10 = 19

*/