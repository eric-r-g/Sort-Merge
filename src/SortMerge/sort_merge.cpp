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
            return a.cols[idx_col] < b.cols[idx_col];
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

Tabela Sort_Merge::juntar_runs(Tabela& tabela_runs, string col_ordenacao){
    //A ideia é pegar 2 runs e juntar elas em uma run maior até sobrar só uma
    
    if(tabela_runs.qtd_pags <= 5) return tabela_runs;

    //acessa o hash para saber o indice da coluna
    int idx_col = tabela_runs.esquema.nome_para_indice[col_ordenacao];

    //cada run tem no maximo 5 paginas inicialmente
    int tamanho_run_atual = 5;
    Tabela atual = tabela_runs;

    //fica em loop ate cobrir a tabela inteira
    while(tamanho_run_atual < atual.qtd_pags){
        Tabela aux;
        aux.esquema = atual.esquema;

        //percorrer de 2 em 2
        for(int i = 0 ; i < atual.qtd_pags ; i += 2*tamanho_run_atual){
            //pegar o comeco e fim de cada run
            int L1 = i, R1 = min(i + tamanho_run_atual, atual.qtd_pags);
            int L2 = R1, R2 = min(i + 2*tamanho_run_atual, atual.qtd_pags);

            if(L2 == atual.qtd_pags){ //nao tem uma segunda run, so copia as paginas
                for(int j = L1 ; j < R1 ; j++){
                    aux.pags.push_back(atual.pags[j]);
                    aux.qtd_pags++;
                }
                break;
            }
            
            //alocar no buffer as 2 runs e a pagina de saida
            int idx_buf_r1 = buffer.carregar_para_memoria(atual.pags[L1]);
            int idx_buf_r2 = buffer.carregar_para_memoria(atual.pags[L2]);
            int idx_buf_saida = buffer.carregar_para_memoria(Pagina());
            int t_r1 = 0, t_r2 = 0, t_saida = 0;

            while(L1 < R1 && L2 < R2){
                Tupla& tp1 = buffer.get_pagina(idx_buf_r1).tuplas[t_r1];
                Tupla& tp2 = buffer.get_pagina(idx_buf_r2).tuplas[t_r2];
                
                //compara as duas
                string val1 = tp1.cols[idx_col], val2 = tp2.cols[idx_col];
                if(val1 <= val2){
                    //atualiza a pagina de saida
                    buffer.get_pagina(idx_buf_saida).tuplas[t_saida] = tp1;
                    t_saida++; t_r1++;
                    //se percorreu todas as tuplas
                    if(t_r1 == buffer.get_pagina(idx_buf_r1).qtd_tuplas_ocup){
                        t_r1 = 0; L1++;
                        buffer.liberar_frame(idx_buf_r1);
                        //atualizar o indice do buffer
                        if(L1 < R1) idx_buf_r1 = buffer.carregar_para_memoria(atual.pags[L1]);
                    }
                }
                else{ //o mesmo processo so que com a outra run
                    buffer.get_pagina(idx_buf_saida).tuplas[t_saida] = tp2;
                    t_saida++; t_r2++;
                    if(t_r2 == buffer.get_pagina(idx_buf_r2).qtd_tuplas_ocup){
                        t_r2 = 0; L2++;
                        buffer.liberar_frame(idx_buf_r2);
                        if(L2 < R2) idx_buf_r2 = buffer.carregar_para_memoria(atual.pags[L2]);
                    }
                }

                //se a pagina de saida encher, grava no disco
                if(t_saida == 12){
                    buffer.get_pagina(idx_buf_saida).qtd_tuplas_ocup = t_saida;
                    aux.pags.push_back(buffer.get_pagina(idx_buf_saida));
                    aux.qtd_pags++;

                    buffer.liberar_frame(idx_buf_saida);
                    idx_buf_saida = buffer.carregar_para_memoria(Pagina());
                    t_saida = 0;
                }
            }

            //copiar as tuplas da run 1 se ainda tiver sobrando
            while(L1 < R1){
                buffer.get_pagina(idx_buf_saida).tuplas[t_saida] = buffer.get_pagina(idx_buf_r1).tuplas[t_r1];
                t_saida++; t_r1++;
                if(t_r1 == buffer.get_pagina(idx_buf_r1).qtd_tuplas_ocup){
                    t_r1 = 0; L1++;
                    buffer.liberar_frame(idx_buf_r1);
                    if(L1 < R1) idx_buf_r1 = buffer.carregar_para_memoria(atual.pags[L1]);
                }

                if(t_saida == 12){
                    buffer.get_pagina(idx_buf_saida).qtd_tuplas_ocup = t_saida;
                    aux.pags.push_back(buffer.get_pagina(idx_buf_saida));
                    aux.qtd_pags++;

                    buffer.liberar_frame(idx_buf_saida);
                    idx_buf_saida = buffer.carregar_para_memoria(Pagina());
                    t_saida = 0;
                }
            }

            //copiar da run 2 se ainda tiver sobrando
            while(L2 < R2){
                buffer.get_pagina(idx_buf_saida).tuplas[t_saida] = buffer.get_pagina(idx_buf_r2).tuplas[t_r2];
                t_saida++; t_r2++;
                if(t_r2 == buffer.get_pagina(idx_buf_r2).qtd_tuplas_ocup){
                    t_r2 = 0; L2++;
                    buffer.liberar_frame(idx_buf_r2);
                    if(L2 < R2) idx_buf_r2 = buffer.carregar_para_memoria(atual.pags[L2]);
                }

                if(t_saida == 12){
                    buffer.get_pagina(idx_buf_saida).qtd_tuplas_ocup = t_saida;
                    aux.pags.push_back(buffer.get_pagina(idx_buf_saida));
                    aux.qtd_pags++;

                    buffer.liberar_frame(idx_buf_saida);
                    idx_buf_saida = buffer.carregar_para_memoria(Pagina());
                    t_saida = 0;
                }
            }

            //grava a ultima pagina se tiver algo nela
            if(t_saida > 0){
                buffer.get_pagina(idx_buf_saida).qtd_tuplas_ocup = t_saida;
                aux.pags.push_back(buffer.get_pagina(idx_buf_saida));
                aux.qtd_pags++;
            }
            buffer.liberar_frame(idx_buf_saida);
        }

        //atualiza as informacoes (atualiza a atual e runs duplicam de tamanho)
        atual = aux;
        tamanho_run_atual *= 2;
    }
    return atual;
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
Tabela Sort_Merge::merge(Tabela& tabela_a, Tabela& tabela_b, string col_juncaoA, string col_juncaoB){
    Tabela saida;
    saida.esquema.qtd_cols = tabela_a.esquema.qtd_cols + tabela_b.esquema.qtd_cols - 1;

    int sz_a = tabela_a.qtd_pags, sz_b = tabela_b.qtd_pags;
    int idx_saida = buffer.carregar_para_memoria(Pagina());
    int ts = 0;
    pair <int, int> pos_b = {0, 0}; // pg_b, tb
    int idx_b = buffer.carregar_para_memoria(tabela_b.pags[pos_b.first]);
    int idx_col_a = tabela_a.esquema.nome_para_indice[col_juncaoA];
    int idx_col_b = tabela_b.esquema.nome_para_indice[col_juncaoB];

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
                if(tp_a.cols[idx_col_a] < tp_b.cols[idx_col_b]) 
                    break;

                // Se for maior o B precisa avançar, o que pode gerar mudanças
                else if(tp_a.cols[idx_col_a] > tp_b.cols[idx_col_b]){
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

                    while(tp_a.cols[idx_col_a] == temp_tp_b.cols[idx_col_b]){
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