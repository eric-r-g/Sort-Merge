#ifndef CLASSES_AUX_H
#define CLASSES_AUX_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

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

#endif