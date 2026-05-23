#include "sort_merge.h"

Sort_Merge::Sort_Merge(){}

Tabela Sort_Merge::gerar_runs(Tabela& tabela_original, string col_ordenacao){
    Tabela runs;
    runs.esquema = tabela_original.esquema;

    //acessa o hash para saber o indice da coluna
    int idx_col = tabela_original.esquema.nome_para_indice[col_ordenacao];
    
    //percorrer todas as paginas da tabela original
    int pagina_atual = 0;
    while(pagina_atual < tabela_original.qtd_pags){
        vector<Tupla> temp; //armazenar as tuplas que vao ser ordenadas

        //carregar as paginas no buffer
        int idx_buffer = 0; //valor inicial, nao representa nada
        while(pagina_atual < tabela_original.qtd_pags){
            idx_buffer = buffer.carregar_para_memoria(tabela_original.pags[pagina_atual]);
            if(idx_buffer == -1) break; //buffer cheio

            Pagina atual = buffer.get_pagina(idx_buffer);
            for(int i = 0 ; i < atual.qtd_tuplas_ocup ; i++){
                temp.push_back(atual.tuplas[i]);
            }

            pagina_atual++;
        }
        //funcao para ordenar as tuplas baseado no indice da coluna
        auto comparar_tuplas = [idx_col](const Tupla&a, const Tupla& b){
            return stoi(a.cols[idx_col]) < stoi(b.cols[idx_col]);
        };
        sort(temp.begin(), temp.end(), comparar_tuplas);

        //realocar as tuplas em novas paginas
        Pagina p;
        for(int i = 0 ; i < (int)temp.size() ; i++){
            p.tuplas[p.qtd_tuplas_ocup] = temp[i];
            p.qtd_tuplas_ocup++;

            if(p.qtd_tuplas_ocup == 12 || i == (int)temp.size() - 1){
                runs.pags.push_back(p);
                runs.qtd_pags++;
                p.qtd_tuplas_ocup = 0;
            }
        }

        //liberar frames do buffer
        for(int i = 0 ; i < 5 ; i++)
            buffer.liberar_frame(i);

        temp.clear();
    }

    return runs;
}