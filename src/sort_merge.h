#ifndef SORT_MERGE_H
#define SORT_MERGE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "buffer_manager.h"

using namespace std;

struct Esquema{
    int qtd_cols;
    unordered_map<string, int> nome_para_indice;
};

struct Tupla{
    vector<string> cols; //tamanho deve ser qtd_cols
};

struct Pagina{
    Tupla tuplas[12];
    int qtd_tuplas_ocup;
};

struct Tabela{
    vector<Pagina> pags;
    int qtd_pags;
    Esquema esquema;
};

class Sort_Merge{
    private:
        Buffer_Manager buffer;
    public:
        Sort_Merge();
        Tabela gerar_runs(Tabela& tabela_original, string col_ordenacao);
};

#endif