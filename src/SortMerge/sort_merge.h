#ifndef SORT_MERGE_H
#define SORT_MERGE_H

#include "../BufferManager/buffer_manager.h"
#include "../classes_aux.h"

class Sort_Merge{
    private:
        Buffer_Manager buffer;
        Tupla juncao_tupla(Tupla& a, Tupla& b, int idx_col);
    public:
        Sort_Merge();
        Tabela gerar_runs(Tabela& tabela_original, string col_ordenacao);
        Tabela juntar_runs(Tabela& tabela_runs, string col_ordenacao);
        Tabela merge(Tabela& tabela_a, Tabela& tabela_b, string col_juncaoA, string col_juncaoB);
};

#endif