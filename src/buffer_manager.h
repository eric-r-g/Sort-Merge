#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include "sort_merge.h"

class Buffer_Manager{
    private:
        Pagina frames[5];
        vector<bool> frame_ocupado;

    public:
        Buffer_Manager();

        int carregar_para_memoria(const Pagina& pagina);
        void liberar_frame(int indice);
        Pagina& get_pagina(int indice);
};

#endif