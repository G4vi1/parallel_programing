#include <omp.h>
#include <iostream>

int main() {

    /*
    Este programa calcula o quadrado de números de 0 a N-1 em paralelo.
    */
    const int N = 8;
    int resultado[N];

    /*
    Aqui nós já criamos a equipe de
    threads + executamos um for, tendo apenas um 
    laço para distribuir o trabalho
    */
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        resultado[i] = i * i; // Calcula o quadrado de i
    }

    // Imprime os resultados
    for (int i = 0; i < N; ++i) {
        std::cout << "resultado[" << i << "] = " << resultado[i] << std::endl;
    }
    return 0;
}

/*
resultado[0] = 0
resultado[1] = 1
resultado[2] = 4
resultado[3] = 9
resultado[4] = 16
resultado[5] = 25
resultado[6] = 36
resultado[7] = 49

analisando o resultado pode-se cravar que a impressão saiu ordenada porque
a impressão rolou depois do laço paralelo, em um segundo laço sequencial

a barreira implicita tbm permitiu que todos os cálculos terminassem antes da impressão
*/