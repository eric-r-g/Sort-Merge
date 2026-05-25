# Trabalho 3 de Banco de Dados - Sort Merge Join

**Dupla:** 
- Eric Rodrigues Gomes
- Paulo Eduardo Conrado Marinho

## Descrição do Projeto
Esse projeto implementa o operador de Junção Sort-Merge em C++ utilizando a técnica de Ordenação Externa. O sistema é composto pelos seguintes itens:
1. classes_aux.h: implementação por meio de structs das abstrações do Banco de Dados.
2. BufferManager: simula um gerenciador de buffer limitado a 5 frames, onde cada página armazena no máximo 12 tuplas.
3. SortMerge: arquivo principal do projeto, com as funções de gerar runs, juntar runs e merge.

## Estrutura de arquivos
Antes de executar o programa, garanta que a organização de diretórios está correta. Os arquivos de dados (como grapes.csv e wines.csv) devem estar no mesmo local onde o executável será rodado. O código fonte segue uma separação modular na pasta src:

    src/main.cpp e src/classes_aux.h (Estruturas base como Tupla, Pagina e Tabela)

    src/BufferManager/: Contém buffer_manager.cpp e buffer_manager.h

    src/SortMerge/: Contém sort_merge.cpp e sort_merge.h

## Como compilar e executar
Para compilar o projeto, abra o terminal na pasta src e execute o comando:
> g++ main.cpp BufferManager/buffer_manager.cpp SortMerge/sort_merge.cpp -o sortmerge

Em seguida, para garantir que as tabelas (.csv) estejam na mesma pasta que o executável, mova o arquivo sortmerge.exe para fora da pasta src (dentro da pasta Sort-Merge) e o execute:
> ./sortmerge