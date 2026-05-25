#include <iostream>
#include <fstream>
#include <sstream>
#include "SortMerge/sort_merge.h"
using namespace std;

// recebe uma linha do csv e já separa na colunas devidas;
vector <string> separa_linha(string& s){
    vector <string> saida;
    bool aspa = false;
    string col_atual = "";

    for(char c : s){
        if(c == ',' && !aspa){
            saida.push_back(col_atual);
            col_atual = "";
        }
        else if (c == '\"')
            aspa = !aspa;
        else 
            col_atual += c;
    }

    return saida;
}

Tabela carregar_csv(string& arquivo){
    Tabela saida;
    ifstream entrada(arquivo);

    if(!entrada.is_open()){
        cout << "Houve um erro ao abrir o arquivo: " + arquivo + "\n";
        cout << "---------------------------------------------\n";
        return saida;
    }

    Esquema e;
    string linha_atual, coluna;

    // lida com cabeçalho
    getline(entrada, linha_atual); 
    int cont = 0;
    vector <string> cabecalho = separa_linha(linha_atual);
    for(string& coluna : cabecalho){
        if (!coluna.empty() && coluna.back() == '\r') {
            coluna.pop_back(); 
        }
        e.nome_para_indice[coluna] = cont++;
    }
    
    Pagina p; int pos_tupla = 0; // para organizar em paginas
    while(getline(entrada, linha_atual)){
        // recebe as colunas
        vector <string> ss = separa_linha(linha_atual);
        Tupla t;
        for(string& coluna : ss){
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

void criar_csv(Tabela& t){
    ofstream saida("saida.csv");

    if(!saida.is_open()){
        cout << "Houve um erro ao criar o arquivo de saida\n";
        cout << "---------------------------------------------\n";
        return;
    }

    int lim = t.esquema.qtd_cols;
    for(Pagina &p : t.pags){
        for(int i = 0; i < p.qtd_tuplas_ocup; i++){
            saida << "\"" << p.tuplas[i].cols[0] << "\"";
            for(int c = 1; c < lim; c++)
                saida << ',' << "\"" << p.tuplas[i].cols[c] << "\"";
            saida << "\n";
        }
    }
}

int main(){
    string tname1, tname2, colA, colB;

    cout << "---------------------------------------------\n";
    cout << "Seja bem vindo!! Gostaria de\n";
    cout << "fazer o teste basico? [S/N]: ";
    char modo; cin >> modo;

    if(modo == 'S'){
        tname1 = "grapes.csv";
        tname2 = "wines.csv";
        colA = "chave_primaria";
        colB = "chave_estrangeira";
    } else {
        cout << "---------------------------------------------\n";
        cout << "Digite o nome da primeira \n";
        cout << "tabela que sera utilizada: ";
        cin >> tname1;
        cout << "---------------------------------------------\n";
        cout << "Digite agora o nome da segunda \n";
        cout << "tabela que sera utilizada: ";
        cin >> tname2;
        cout << "---------------------------------------------\n";
        cout << "Digite agora o nome da coluna que sera \n";
        cout << "utilizada para juncao na tabela 1: ";
        cin >> colA;
        cout << "---------------------------------------------\n";
        cout << "Digite agora o nome da coluna que sera \n";
        cout << "utilizada para juncao na tabela 2: ";
        cin >> colB;
        cout << "---------------------------------------------\n";
    }

    Tabela A = carregar_csv(tname1);
    if(A.qtd_pags == 0) return 1;
    Tabela B = carregar_csv(tname2);
    if(B.qtd_pags == 0) return 1;
    
    Sort_Merge SMJ;

    Tabela A_runs = SMJ.gerar_runs(A, colA);
    Tabela B_runs = SMJ.gerar_runs(B, colB);
    Tabela A_ord = SMJ.juntar_runs(A_runs, colA);
    Tabela B_ord = SMJ.juntar_runs(B_runs, colB);
    Tabela C = SMJ.merge(A_ord, B_ord, colA, colB);
    cout << "Juncao concluida com sucesso! \n";
    cout << "---------------------------------------------\n";
    
    criar_csv(C);
}