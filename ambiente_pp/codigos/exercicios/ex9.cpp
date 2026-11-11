#include <iostream>
#include <omp.h>

int main() {
    int resultado[8];

    #pragma omp parallel num_threads(4)
    {
        #pragma omp for
        for (int i = 0; i < 8; ++i) {
            resultado[i] = i * 2; // Calcula o dobro de i
        }
    }

    for (int i = 0; i < 8; ++i) {
        std::cout << "resultado[" << i << "] = " << resultado[i] << std::endl;
    }
    return 0;
}