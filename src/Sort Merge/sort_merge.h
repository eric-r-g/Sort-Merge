#ifndef SORT_MERGE_H
#define SORT_MERGE_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include "buffer_manager.h"
#include "classes_aux.h"

using namespace std;

class Sort_Merge{
    private:
        Buffer_Manager buffer;
    public:
        Sort_Merge();
        Tabela gerar_runs(Tabela& tabela_original, string col_ordenacao);
        Tabela merge(Tabela& tabela_a, Tabela& tabela_b, string col_juncao);
};

#endif