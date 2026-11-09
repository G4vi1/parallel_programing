#include <iostream>
#include <omp.h>

int main() {
    const int N = 100;
    int soma = 0;

    /* A diretiva `#pragma omp parallel for` indica que o
    loop `for` será executado em paralelo.
    */

    /*
    reduction(+:soma) é uma cláusula do OpenMP que especifica que 
    a variável soma será usada em uma operação de redução.
    A operação de redução é uma técnica usada para 
    combinar os resultados de várias threads em uma única
    */

    #pragma omp parallel for reduction(+:soma)
    for (int i = 1; i <= N; ++i) {
        soma += i;
    }

    std::cout << "Soma de 1 até 100: " << soma << '\n';

    return 0;
}