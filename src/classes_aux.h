#ifndef CLASSES_AUX_H
#define CLASSES_AUX_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

using namespace std;

struct Esquema{
    int qtd_cols = 0;
    unordered_map<string, int> nome_para_indice;
};

struct Tupla{
    vector<string> cols; //tamanho deve ser qtd_cols
};

struct Pagina{
    Tupla tuplas[12];
    int qtd_tuplas_ocup = 0;
};

struct Tabela{
    vector<Pagina> pags;
    int qtd_pags = 0;
    Esquema esquema;
};

#endif