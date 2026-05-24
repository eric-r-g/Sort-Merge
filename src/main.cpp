#include <iostream>
#include <fstream>
#include <sstream>
#include "SortMerge/sort_merge.h"
using namespace std;

bool erro = false;

Tabela carregar_csv(string& arquivo){
    Tabela saida;
    ifstream entrada(arquivo);

    if(!entrada.is_open()){
        cout << "Houve um erro ao abrir o arquivo: " + arquivo + "\n";
        cout << "---------------------------------------------\n";
        erro = true;
        return saida;
    }

    Esquema e;
    string linha_atual, coluna;

    // lida com cabeçalho
    getline(entrada, linha_atual); 
    int cont = 0;
    stringstream cabecalho(linha_atual);
    while(getline(cabecalho, coluna, ',')){
        if (!coluna.empty() && coluna.back() == '\r') {
            coluna.pop_back(); 
        }
        e.nome_para_indice[coluna] = cont++;
    }
    
    Pagina p; int pos_tupla = 0; // para organizar em paginas
    while(getline(entrada, linha_atual)){
        // recebe as colunas
        stringstream ss(linha_atual);
        Tupla t;
        while(getline(ss, coluna, ',')){
            if (!coluna.empty() && coluna.back() == '\r') {
                coluna.pop_back(); 
            }
            t.cols.push_back(coluna);
        }

        // coloca a tupla
        if(e.qtd_cols == 0) e.qtd_cols = t.cols.size();
        p.tuplas[pos_tupla] =  t;
        p.qtd_tuplas_ocup++; pos_tupla++;
        if(pos_tupla == 12){
            saida.pags.push_back(p);
            p = Pagina();
            pos_tupla = 0;
        }
    }

    if(pos_tupla != 0)  
        saida.pags.push_back(p);
    saida.qtd_pags = saida.pags.size();
    saida.esquema = e;
    return saida;
}   

int main(){
    string tname1, tname2, col1, col2;
    cout << "---------------------------------------------\n";
    cout << "Seja bem vindo!! Digite o nome da primeira \n";
    cout << "tabela que será utilizada: ";
    cin >> tname1;
    cout << "---------------------------------------------\n";
    cout << "Digite agora o nome da segunda \n";
    cout << "tabela que será utilizada: ";
    cin >> tname2;
    cout << "---------------------------------------------\n";
    cout << "Digite agora o nome da coluna que será \n";
    cout << "utilizada para juncao na tabela 1: ";
    cin >> col1;
    cout << "---------------------------------------------\n";
    cout << "Digite agora o nome da coluna que será \n";
    cout << "utilizada para juncao na tabela 1: ";
    cin >> col2;
    cout << "---------------------------------------------\n";

    Tabela A = carregar_csv(tname1);
    if(erro) return 1;
    Tabela B = carregar_csv(tname2);
    if(erro) return 1;
}