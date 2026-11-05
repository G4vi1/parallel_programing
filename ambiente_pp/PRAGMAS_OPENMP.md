# Guia de `#pragma` e OpenMP

Este guia explica as diretivas e cláusulas encontradas nos exemplos de
`codigos/01_openmp`, incluindo as mencionadas em comentários. Não é um catálogo
de todas as diretivas da especificação OpenMP. Os trechos menores são exemplos
para inserir em um programa; ao final há um programa completo.

## 1. O que é `#pragma`?

Em C e C++, `#pragma` é uma diretiva que transmite instruções ao compilador.
Seu significado depende do que vem depois: `#pragma` sozinho não significa
“executar em paralelo”. Por exemplo, `#pragma once`, comum em cabeçalhos,
tem outra finalidade: evitar a inclusão repetida do mesmo cabeçalho em uma
unidade de tradução, nos compiladores que oferecem esse recurso.

No OpenMP, a estrutura básica é:

```cpp
#pragma omp diretiva cláusulas
```

Leia esta linha por partes:

```cpp
#pragma omp parallel for reduction(+:soma) schedule(static)
```

| Parte | Significado |
|---|---|
| `#pragma` | Introduz a instrução para o compilador. |
| `omp` | Identifica que a instrução pertence ao OpenMP. |
| `parallel for` | Combina a criação de uma equipe de threads com a divisão de um laço. |
| `reduction(+:soma)` | Configura uma soma usando acumuladores privados e combinação dos resultados. |
| `schedule(static)` | Configura a distribuição das iterações. |

Uma **diretiva** determina uma ação; uma **cláusula** ajusta seu comportamento.
`reduction` e `schedule` nessa linha são cláusulas, não diretivas independentes.
Normalmente o bloco ou laço logo abaixo é o código associado à diretiva.
`barrier`, por exemplo, é uma diretiva independente e não recebe um bloco.

OpenMP usa threads que compartilham a memória do processo. Uma thread é um
fluxo de execução; várias podem progredir concorrentemente. Ter mais threads
não garante executar mais rápido: há custos de coordenação e limites de CPU
e de acesso à memória.

## 2. Criar threads e distribuir trabalho

### `parallel`: formar uma equipe de threads

```cpp
#pragma omp parallel
{
    int id = omp_get_thread_num();
    // Cada thread executa este bloco e possui seu próprio id local.
}
```

Cada thread da equipe executa o bloco. `parallel` sozinho não divide um laço:
se houver um `for` comum dentro dele, cada thread poderá executar o laço inteiro.
Há uma barreira implícita ao terminar a região paralela; depois dela, a thread
que iniciou a região continua o código externo.

Veja: [000_hello.cpp](codigos/01_openmp/000_hello.cpp) e
[001_info.cpp](codigos/01_openmp/001_info.cpp).
Referência: [parallel na especificação](https://www.openmp.org/spec-html/5.2/openmpse57.html).

### `for`: dividir iterações entre threads de uma equipe existente

```cpp
#pragma omp parallel
{
    #pragma omp for
    for (int i = 0; i < N; ++i) {
        a[i] = i * 2;
    }
}
```

O `omp for` distribui as iterações entre as threads. Cada iteração é executada
uma vez por uma das threads. Ele não cria uma equipe nova e deve ser encontrado
pelas threads da equipe de forma compatível, sem colocar apenas uma delas para
entrar nesse laço. Por padrão, todas esperam ao final do trabalho distribuído.

Use em laços com iterações independentes. Uma dependência como
`a[i] = a[i - 1] + 1` exige outra análise: acrescentar um pragma não resolve
a dependência entre os elementos.

Veja: [005_loop_for_paralell_time.cpp](codigos/01_openmp/005_loop_for_paralell_time.cpp).

### `parallel for`: criar a equipe e dividir o laço

```cpp
#pragma omp parallel for
for (int i = 0; i < N; ++i) {
    a[i] = x[i] * x[i];
}
```

É a forma combinada para um laço paralelo. Para este exemplo simples, produz
o mesmo efeito de escrever `parallel` com um `omp for` dentro. Ao terminar a
região, todas as iterações foram concluídas.

Veja: [004_loop_for_paralell.cpp](codigos/01_openmp/004_loop_for_paralell.cpp).

### `sections` e `section`: distribuir blocos de tarefas diferentes

```cpp
#pragma omp parallel sections
{
    #pragma omp section
    { tarefa_a(); }

    #pragma omp section
    { tarefa_b(); }
}
```

`sections` agrupa trabalhos diferentes; cada `section` identifica um deles.
Cada seção é executada uma vez por uma thread. Uma thread pode executar mais
de uma seção, e não existe garantia de que uma seção ficará com um ID específico.
`sections` sozinho usa a equipe existente; `parallel sections` também cria a
equipe. Há espera no final de `sections`, salvo uso válido de `nowait`.

Use para tarefas independentes. Se `tarefa_b` precisa do resultado de
`tarefa_a`, esse exemplo não estabelece a ordem necessária.

Veja: [008_sections_0.1.cpp](codigos/01_openmp/008_sections_0.1.cpp).
Apesar do nome, `008_sections_0.0.cpp` usa `parallel` e testes do ID da thread,
não a diretiva `sections`.

## 3. Evitar conflitos em dados compartilhados

Uma **condição de corrida** pode ocorrer quando threads acessam a mesma posição
de memória sem a sincronização necessária e pelo menos uma escreve nela.
Por exemplo, `soma += valor` envolve ler, calcular e escrever; duas threads podem
ler o mesmo valor antigo e perder uma das atualizações.

### `critical`: uma thread por vez em um bloco protegido

```cpp
#pragma omp critical
{
    soma += valor;
    quantidade++;
}
```

Serve para proteger um bloco com várias operações. As threads executam o bloco
uma por vez, sem ordem determinada. Não significa “executar apenas uma vez”:
cada thread que chega ali ainda executa seu trabalho quando conseguir entrar.
Também não é uma barreira que reúne toda a equipe.

É possível nomear a proteção:

```cpp
#pragma omp critical(pares)
{ soma_pares += valor; }
```

Regiões com o mesmo nome compartilham a exclusão mútua. Nomes diferentes permitem
proteções independentes. Regiões `critical` sem nome compartilham uma mesma
proteção sem nome; por isso, podem serializar trechos sem relação entre si.
Todos os acessos conflitantes ao recurso precisam de proteção compatível.

Veja: [006_sincronizacao_0.1.cpp](codigos/01_openmp/006_sincronizacao_0.1.cpp) e
[006_sincronizacao_bhaskara_critical.cpp](codigos/01_openmp/006_sincronizacao_bhaskara_critical.cpp).

### `atomic`: proteger uma operação de memória compatível

```cpp
#pragma omp atomic
soma += valor;
```

Nesta forma, protege a atualização da variável `soma`. É adequado para uma
operação simples admitida pela sintaxe do OpenMP, como acumular ou incrementar.
Não protege um bloco arbitrário nem todas as leituras feitas para calcular
`valor`. Outros acessos conflitantes a `soma` também precisam de sincronização.

Frequentemente tem custo menor que `critical`, mas isso depende da operação e
da implementação. A forma sem cláusula do exemplo é uma atualização; existem
também formas explícitas como `atomic read`, `write` e `capture`, para outras
operações atômicas previstas na especificação.

Veja: [006_sincronizacao_0.2.cpp](codigos/01_openmp/006_sincronizacao_0.2.cpp).

### `reduction`: acumular resultados sem disputar a variável a cada iteração

```cpp
double soma = 0.0;
#pragma omp parallel for reduction(+:soma)
for (int i = 0; i < N; ++i) {
    soma += valores[i];
}
```

É uma cláusula. O OpenMP cria cópias privadas, inicializa-as de acordo com o
operador e combina seus resultados com o valor original ao final. Para somar,
comece normalmente com a variável original em zero; para multiplicar, em um.

| Cláusula | Uso típico |
|---|---|
| `reduction(+:soma)` | Somar valores ou contar ocorrências. |
| `reduction(*:produto)` | Multiplicar valores. |
| `reduction(\|\|:encontrou)` | Verificar se pelo menos um caso atende à condição. |
| `reduction(&&:todos_validos)` | Verificar se todos atendem à condição. |
| `reduction(min:menor)` / `reduction(max:maior)` | Obter extremos, com valor original inicial adequado. |

Inicialize `encontrou` com `false` e `todos_validos` com `true`. É possível usar
mais de uma redução na mesma diretiva. A ordem de combinação não é fixa:
resultados em ponto flutuante podem variar nas últimas casas decimais.

Veja os arquivos `007_reduction_0.0.cpp`, `007_reduction_0.1.cpp` e
`007_reduction_0.2.cpp` em `codigos/01_openmp`.
Referência: [reduction na especificação](https://www.openmp.org/spec-html/5.2/openmpsu52.html).

### `declare reduction`: definir uma redução própria

É uma diretiva declarativa, mencionada nos comentários de `007_reduction_0.0.cpp`.
Serve para definir como combinar um tipo ou operação personalizados e, quando
necessário, como inicializar suas cópias privadas. Depois, seu identificador
pode ser usado em `reduction(...)`. Não cria threads nem executa a redução por
si só; é um recurso para quando os operadores disponíveis não atendem ao caso.

## 4. Coordenar etapas e escolher quem executa

### `barrier`: esperar toda a equipe chegar ao mesmo ponto

```cpp
#pragma omp parallel
{
    preparar_parte();
    #pragma omp barrier
    consumir_dados_preparados();
}
```

Cada thread espera até todas chegarem à barreira. Isso separa fases e estabelece
a sincronização necessária entre elas. Não corrige uma corrida que já aconteceu
em `preparar_parte()`.

As threads da equipe devem encontrar a mesma sequência de barreiras. Colocar
uma barreira dentro de `if (id == 0)`, por exemplo, deixa as demais sem participar
e pode travar o programa. Também há restrições de aninhamento: não coloque uma
barreira dentro de `critical`, `single` ou do corpo de um `omp for`.

Veja: [006_sincronizacao_0.3.cpp](codigos/01_openmp/006_sincronizacao_0.3.cpp).
Nesse exemplo, a barreira logo depois de um `omp for` sem `nowait` é redundante:
o laço já possui uma barreira implícita.

### `ordered`: executar um trecho na ordem lógica das iterações

```cpp
#pragma omp parallel for ordered
for (int i = 0; i < 10; ++i) {
    int resultado = i * i;
    #pragma omp ordered
    { std::cout << i << ": " << resultado << '\n'; }
}
```

Na forma básica usada nas aulas, a cláusula `ordered` no laço permite usar a
diretiva `ordered` no trecho interno. Esse trecho segue a ordem do laço
sequencial. O cálculo fora dele pode continuar em paralelo.

`critical` impede simultaneidade, mas não garante ordem; `ordered` garante
a ordem das iterações naquele trecho. Quanto mais trabalho ficar dentro dele,
menor será a oportunidade de paralelismo.

Veja: [006_sincronizacao_0.4.cpp](codigos/01_openmp/006_sincronizacao_0.4.cpp).

### `single`: uma thread executa o bloco uma vez

```cpp
#pragma omp parallel
{
    #pragma omp single
    { std::cout << "Equipe iniciada\n"; }
    // As threads passam daqui após o single terminar.
}
```

Uma thread escolhida entre as participantes executa o bloco; não há garantia
de ser a thread 0. As demais não executam esse bloco e esperam no final,
salvo uso de `single nowait`. É útil para inicialização ou uma mensagem única.

Veja: [005_loop_for_paralell_time.cpp](codigos/01_openmp/005_loop_for_paralell_time.cpp).

### `master` e `masked`: selecionar a thread executora

```cpp
#pragma omp master
{ /* somente a thread 0 da equipe atual */ }
```

`master` seleciona a thread principal da equipe atual e não tem barreira
implícita. As outras podem continuar antes de o bloco terminar.

Nos compiladores com suporte à versão correspondente do OpenMP, prefira a
forma moderna `masked`, que sem filtro seleciona a thread 0:

```cpp
#pragma omp masked
{ /* thread 0 */ }

#pragma omp masked filter(2)
{ /* thread com ID 2, se existir nesta equipe */ }
```

`masked` também não tem barreira implícita. `master` está obsoleto a partir do
OpenMP 5.1, embora continue presente em códigos existentes. Escolher uma thread
não significa sincronizar o resultado dela com as outras.

Veja: [010_selecionar_thread.cpp](codigos/01_openmp/010_selecionar_thread.cpp).
Referência: [masked na especificação](https://www.openmp.org/spec-html/5.2/openmpse61.html).

## 5. Cláusulas para configurar os pragmas

### `num_threads(n)`: solicitar o tamanho da equipe

```cpp
#pragma omp parallel num_threads(4)
{ /* trabalho */ }
```

Solicita quatro threads. O tamanho efetivo depende das regras e limites do
ambiente OpenMP; confira-o com `omp_get_num_threads()` dentro da região.
Não significa “usar quatro núcleos exclusivos”.

### `schedule(...)`: distribuir iterações

| Forma | Como distribui | Quando considerar |
|---|---|---|
| `schedule(static)` | Distribuição pré-definida em blocos aproximadamente equilibrados. | Iterações com custos parecidos. |
| `schedule(static, 8)` | Blocos de oito iterações atribuídos ciclicamente às threads. | Controle do tamanho dos blocos. |
| `schedule(dynamic, 8)` | Threads solicitam novos blocos de oito conforme terminam. | Custos irregulares, aceitando coordenação adicional. |
| `schedule(guided)` | Blocos tendem a diminuir durante a distribuição. | Balancear trabalho irregular com menos pedidos que blocos sempre pequenos. |

O escalonamento não resolve dependências entre iterações nem garante a ordem
de impressão. Veja [04_schedule_mandelbrot.c](codigos/01_openmp/04_schedule_mandelbrot.c).

### `nowait`: retirar uma espera implícita onde permitido

```cpp
#pragma omp parallel
{
    #pragma omp for nowait
    for (int i = 0; i < N; ++i) {
        a[i] = i;
    }
    trabalho_independente();
    #pragma omp barrier
    // Agora os resultados completos de a podem ser consumidos.
}
```

Ao terminar sua parte, uma thread pode avançar sem esperar as demais no fim do
`for`. Use apenas se o trabalho seguinte puder começar com segurança. Nas formas
das aulas, também se aplica a `single` e `sections`. Não remove a barreira final
de `parallel` e não deve ser acrescentado a `parallel for` como se fosse
equivalente a `for nowait` dentro de uma região paralela.

### `collapse(2)`: distribuir as combinações de dois laços

```cpp
#pragma omp parallel for collapse(2)
for (int i = 0; i < linhas; ++i) {
    for (int j = 0; j < colunas; ++j) {
        matriz[i][j] = i + j;
    }
}
```

Faz os dois níveis participarem do espaço de iterações distribuído. Pode ajudar
quando só o laço externo oferece pouco trabalho. Os laços devem obedecer à forma
canônica e às restrições do OpenMP; as combinações devem poder ser executadas
sem dependências conflitantes.

É uma variação mencionada em [011_matriz_paralela.cpp](codigos/01_openmp/011_matriz_paralela.cpp).

### `shared`, `private`, `firstprivate` e `default(none)`

São complementos úteis para entender quais dados cada thread utiliza:

| Cláusula | Efeito |
|---|---|
| `shared(x)` | As threads acessam o mesmo objeto; não acrescenta proteção contra corridas. |
| `private(x)` | Cada participante tem uma cópia independente; para um escalar, não copia o valor original. Atribua antes de ler. |
| `firstprivate(x)` | Cada cópia privada começa com o valor anterior de `x`. |
| `default(none)` | Exige explicitar os atributos das variáveis sem atributo predeterminado ou outra exceção prevista. Ajuda a revisar os dados. |

Nos exemplos comuns, variáveis declaradas dentro do bloco paralelo são locais
à thread; variáveis externas costumam ser compartilhadas. O índice do `omp for`
tem atributo privado predeterminado. Essas regras não dispensam revisar cada
acesso: um ponteiro privado, por exemplo, ainda pode apontar para dados compartilhados.

## 6. Funções OpenMP não são pragmas

| Função | Para que serve |
|---|---|
| `omp_get_thread_num()` | Obter o ID da thread na equipe atual. |
| `omp_get_num_threads()` | Obter o tamanho da equipe atual; fora de uma região paralela ativa retorna 1. |
| `omp_get_max_threads()` | Obter um limite para a equipe de uma próxima região paralela sem `num_threads`, conforme o estado atual. Não mede núcleos físicos. |
| `omp_get_wtime()` | Ler um relógio em segundos; meça início e fim na mesma thread. |
| `omp_init_lock()` | Inicializar um lock. |
| `omp_set_lock()` | Adquirir o lock, esperando se necessário. |
| `omp_unset_lock()` | Liberar o lock adquirido. |
| `omp_destroy_lock()` | Destruir um lock após seu uso. |

Locks são a proteção explícita usada em `006_sincronizacao_0.5.cpp`. Diferentemente
de `critical`, exigem gerenciar a aquisição e liberação. Esquecer de liberar ou
adquirir múltiplos locks em ordens incompatíveis pode travar o programa.

## 7. Como escolher

| Objetivo | Recurso |
|---|---|
| Executar um bloco com uma equipe | `parallel` |
| Dividir um laço independente | `parallel for`, ou `for` dentro de `parallel` |
| Distribuir blocos diferentes e independentes | `sections` / `section` |
| Somar, contar ou combinar resultados | `reduction` |
| Proteger uma atualização simples compatível | `atomic` |
| Proteger várias operações em conjunto | `critical` ou locks |
| Esperar a conclusão de uma fase por toda a equipe | `barrier` |
| Executar um trecho na ordem das iterações | `ordered` |
| Executar um bloco uma vez, com espera ao final | `single` |
| Executar na thread principal, sem espera automática | `masked` ou o legado `master` |

## 8. Exemplo completo para compilar

```cpp
#include <iostream>
#include <omp.h>

int main() {
    constexpr int N = 100;
    long long soma = 0;
    int threads_usadas = 0;

    #pragma omp parallel num_threads(4) default(none) shared(soma, threads_usadas)
    {
        #pragma omp single
        { threads_usadas = omp_get_num_threads(); }

        #pragma omp for reduction(+:soma) schedule(static)
        for (int i = 1; i <= N; ++i) {
            soma += i;
        }
    }

    std::cout << "Threads: " << threads_usadas << '\n';
    std::cout << "Soma: " << soma << '\n';  // Esperado: 5050.
}
```

Salve o trecho como `exemplo_pragmas.cpp`. Com GCC no Linux ou no ambiente da
disciplina, compile e execute:

```bash
g++ -std=c++14 -O2 -fopenmp exemplo_pragmas.cpp -o exemplo_pragmas
./exemplo_pragmas
```

`-fopenmp` ativa o suporte do GCC a OpenMP e inclui a biblioteca necessária na
ligação. `#include <omp.h>` declara as funções da biblioteca. Apenas incluir o
cabeçalho não ativa o processamento das diretivas. Compiladores e versões
diferentes podem oferecer suporte a conjuntos diferentes de recursos.

## Referências para aprofundamento

- [Especificação OpenMP 5.2](https://www.openmp.org/spec-html/5.2/openmp.html): índice das diretivas, cláusulas e rotinas.
- [Resumo do LLNL](https://hpc-tutorials.llnl.gov/openmp/clauses_directives_summary/): consulta rápida de diretivas e cláusulas.
- [Diretivas na documentação Microsoft](https://learn.microsoft.com/en-us/cpp/parallel/openmp/2-directives?view=msvc-160): explicações das formas clássicas; o suporte descrito é específico desse compilador.

O foco deste guia é a semântica dos exemplos locais. Para recursos mais novos,
confira também a versão de OpenMP suportada pelo compilador que você utiliza.
