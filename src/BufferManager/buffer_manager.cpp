#include "buffer_manager.h"

Buffer_Manager::Buffer_Manager(){
    frame_ocupado.assign(5, 0); //inicialmente, todos os frames estao livres
}

int Buffer_Manager::carregar_para_memoria(const Pagina& pagina){
    //retorna o indice do frame que a pagina foi carregada
    for(int i = 0 ; i < 5 ; i++){
        if(frame_ocupado[i] == 0){
            frame_ocupado[i] = 1;
            frames[i] = pagina;
            return i;
        }
    }
    return -1; //buffer cheio, retorna -1
}

void Buffer_Manager::liberar_frame(int indice){
    if(indice < 0 || indice >= 5) return ; //indice invalido
    frame_ocupado[indice] = false;
    frames[indice].qtd_tuplas_ocup = 0;
}

Pagina& Buffer_Manager::get_pagina(int indice){
    if(indice < 0 || indice >= 5)
        cout << "Acesso Invalido" << endl;
    else
        return frames[indice];
}