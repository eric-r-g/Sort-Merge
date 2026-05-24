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

// juntas a colunas de ambos com exceção na tupla b da coluna idx_col
Tupla Sort_Merge::juncao_tupla(Tupla& a, Tupla& b, int idx_col){
    Tupla c;
    for(int i = 0; i < a.cols.size(); i++) c.cols.push_back(a.cols[i]);
    for(int i = 0; i < b.cols.size(); i++){
        if(i == idx_col) continue;
        c.cols.push_back(b.cols[i]);
    }
    return c;
}

// o merge ficará com 1 frame de entrada para o A, 1 Frames de entrada para o B, 1 Frames de Saida, 1 reserva 
Tabela Sort_Merge::merge(Tabela& tabela_a, Tabela& tabela_b, string col_juncao){
    Tabela saida;
    int sz_a = tabela_a.qtd_pags, sz_b = tabela_b.qtd_pags;

    int idx_saida = buffer.carregar_para_memoria(Pagina());
    int ts = 0;
    pair <int, int> pos_b = {0, 0}; // pg_b, tb
    int idx_b = buffer.carregar_para_memoria(tabela_b.pags[pos_b.first]);
    int idx_col_a = tabela_a.esquema.nome_para_indice[col_juncao];
    int idx_col_b = tabela_b.esquema.nome_para_indice[col_juncao];

    // para cada uma das tuplas de A, ele faz o processo
    for(int pg_a = 0; pg_a < sz_a; pg_a++){
        int idx_a = buffer.carregar_para_memoria(tabela_a.pags[pg_a]);
        int tuplas_a = buffer.get_pagina(idx_a).qtd_tuplas_ocup;

        for(int ta = 0; ta < tuplas_a; ta++){
            // vai comparar as tuplas de A com as tuplas de B
            while(true){
                Tupla& tp_a = buffer.get_pagina(idx_a).tuplas[ta];
                Tupla& tp_b = buffer.get_pagina(idx_b).tuplas[pos_b.second];

                // Se for menor o A precisa avançar
                if(stoi(tp_a.cols[idx_col_a]) < stoi(tp_b.cols[idx_col_b])) 
                    break;

                // Se for maior o B precisa avançar, o que pode gerar mudanças
                else if(stoi(tp_a.cols[idx_col_a]) > stoi(tp_b.cols[idx_col_b])){
                    pos_b.second++;
                    if(pos_b.second >= buffer.get_pagina(idx_b).qtd_tuplas_ocup){
                        pos_b.second = 0;
                        pos_b.first++;

                        if(pos_b.first == sz_b) break;
                        buffer.liberar_frame(idx_b);
                        idx_b = buffer.carregar_para_memoria(tabela_b.pags[pos_b.first]);
                    }
                }
                // nesse caso são iguais, e como pode ter uma relação muitos para muitos, 
                // ele precisa avançar o maximo que consegue
                else {
                    pair <int, int> temp_pos_b = pos_b;
                    int temp_idx_b = idx_b;
                    Tupla& temp_tp_b = tp_b;

                    while(stoi(tp_a.cols[idx_col_a]) == stoi(temp_tp_b.cols[idx_col_b])){
                        buffer.get_pagina(idx_saida).tuplas[ts++] = juncao_tupla(tp_a, temp_tp_b, idx_col_b);

                        // guardar a pagina na tabela de saida
                        if(ts == 12){
                            buffer.get_pagina(idx_saida).qtd_tuplas_ocup = ts;
                            saida.pags.push_back(buffer.get_pagina(idx_saida));
                            buffer.liberar_frame(idx_saida);
                            idx_saida = buffer.carregar_para_memoria(Pagina());
                            ts = 0;
                        }

                        temp_pos_b.second++;
                        if(temp_pos_b.second == buffer.get_pagina(temp_idx_b).qtd_tuplas_ocup){
                            temp_pos_b.second = 0;
                            temp_pos_b.first++;
                            if(temp_pos_b.first == sz_b) break;

                            if(temp_idx_b != idx_b) buffer.liberar_frame(temp_idx_b);
                            temp_idx_b = buffer.carregar_para_memoria(tabela_b.pags[temp_pos_b.first]);
                        }
                        temp_tp_b = buffer.get_pagina(temp_idx_b).tuplas[temp_pos_b.second];
                    }

                    if(temp_idx_b != idx_b) buffer.liberar_frame(temp_idx_b);
                    break;
                }
            }
            if(pos_b.first == sz_b) break;
        }

        buffer.liberar_frame(idx_a);
        if(pos_b.first == sz_b) break;
    }

    if(ts != 0){
        buffer.get_pagina(idx_saida).qtd_tuplas_ocup = ts;
        saida.pags.push_back(buffer.get_pagina(idx_saida));
        buffer.liberar_frame(idx_saida);
    }

    buffer.liberar_frame(idx_b);
    return saida;
}