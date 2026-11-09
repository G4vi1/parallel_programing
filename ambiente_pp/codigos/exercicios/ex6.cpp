#include <iostream>
#include <vector>
#include <omp.h>

int main() {

    const int N = 8;

    std::vector<int> resultado(N);
    std::vector<int> responsavel(N);

    // A diretiva `#pragma omp parallel for` indica que o loop `for` será executado em paralelo.

    #pragma omp parallel for schedule(static) num_threads(4)
    for (int i = 0; i < N; ++i) {
        resultado[i] = i * i; // Calcula o quadrado de i
        responsavel[i] = omp_get_thread_num(); // Armazena o ID da thread
    }

    for (int i = 0; i < N; ++i) {
        std::cout << "resultado[" << i << "] = " << resultado[i]
                  << ", calculado pela thread " << responsavel[i] << std::endl;
    }
    return 0;
}

/*
resultado[0] = 0, calculado pela thread 0
resultado[1] = 1, calculado pela thread 0
resultado[2] = 4, calculado pela thread 1
resultado[3] = 9, calculado pela thread 1
resultado[4] = 16, calculado pela thread 2
resultado[5] = 25, calculado pela thread 2
resultado[6] = 36, calculado pela thread 3
resultado[7] = 49, calculado pela thread 3

analise: O resultado mostra que o cálculo do quadrado de cada 
número foi distribuído entre as threads disponíveis. 
Cada thread foi responsável por calcular um bloco 
contíguo de iterações, conforme especificado pela cláusula 
`schedule(static)`. A impressão dos resultados ocorre 
após a execução paralela, garantindo que todos os 
cálculos tenham sido concluídos antes da exibição.
*/