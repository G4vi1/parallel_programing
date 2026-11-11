#include <iostream>
#include <omp.h>

int main() {
    #pragma omp parallel num_threads(4)
    {
        int id = omp_get_thread_num();
        #pragma omp critical
        {
            std::cout << "Thread " << id << " está executando a região crítica." << std::endl;
        }
    }
    return 0;
}