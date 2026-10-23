# Ambiente de Laboratório — Programação Paralela (CCO085)

**IESB 2026/2 · Prof. Rodrigo Gonçalves Pinto**

Este é o ambiente oficial da disciplina. Todas as medições reportadas no
Projeto Acadêmico IESB devem ser feitas aqui, para que os resultados sejam
comparáveis entre os trios.

## 1. Subir o ambiente

Requisito: Docker e Docker Compose instalados.

```bash
docker compose build      # constrói a imagem (só na primeira vez)
docker compose run --rm pp-lab   # abre um shell dentro do container
```

Dentro do container você cai em `/lab`, com as pastas `codigos/` e `python/`
montadas do seu computador — o que você editar fora aparece dentro na hora.

## 2. Compilar

Dentro do container:

```bash
make            # compila tudo (base + OpenMP + MPI)
make openmp     # só os exemplos OpenMP
make mpi        # só os exemplos MPI
make clean      # remove os binários
```

## 3. Executar os exemplos

```bash
# baseline sequencial
./codigos/00_base/baseline_pi 100000000

# OpenMP — controla o número de threads pela variável de ambiente
OMP_NUM_THREADS=4 ./codigos/01_openmp/01_hello_openmp
./codigos/01_openmp/02_pi_openmp 100000000 4

# MPI — controla o número de processos com -np
mpirun -np 4 ./codigos/02_mpi/01_hello_mpi
mpirun -np 4 ./codigos/02_mpi/03_coletivas_pi 100000000
```

Se você estiver rodando como root dentro do container e o `mpirun` reclamar,
as variáveis `OMPI_ALLOW_RUN_AS_ROOT` já estão definidas na imagem.

## 4. Coletar dados de escalabilidade

```bash
./scripts/medir_speedup.sh 100000000 5
# gera resultados/openmp_escalabilidade.csv
```

O script descarta a primeira repetição de cada configuração (aquecimento) e
grava as demais em CSV, no formato esperado pelo relatório do projeto.

## 5. CUDA e Python

- **CUDA:** não roda neste container (é CPU). Use o notebook
  `notebooks/cuda_colab_CCO085.ipynb` no Google Colab, com GPU T4.
- **GIL / multiprocessing:** `python3 python/gil_demo.py`.
- **PySpark:** demonstrado em notebook no bloco de computação distribuída.

## AVISO IMPORTANTE sobre número de núcleos

Speedup só aparece se a máquina tiver **mais de um núcleo de CPU**. Confira com:

```bash
nproc
```

Se `nproc` retornar 1, os programas paralelos ainda funcionam e dão o
resultado correto, mas **não vão acelerar** — e a condição de corrida do
exemplo `03_condicao_corrida` pode não se manifestar, dando a falsa impressão
de que o código sem proteção está certo. Ele não está: em máquina com vários
núcleos, o valor "sem proteção" sai errado. Faça suas medições numa máquina
com vários núcleos (as do laboratório têm).

## Estrutura

```
ambiente_pp/
├── Dockerfile
├── docker-compose.yml
├── Makefile
├── codigos/
│   ├── 00_base/       baseline sequencial
│   ├── 01_openmp/     memória compartilhada
│   └── 02_mpi/        memória distribuída
├── python/            GIL e multiprocessing
├── notebooks/         CUDA no Colab
├── scripts/           coleta de dados de desempenho
└── resultados/        saída das medições (CSV)
```
