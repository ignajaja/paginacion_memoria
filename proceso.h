#pragma once

#include <string>
#include <iostream>
#include "pagina.h"
#include "splayTree.h"
#include "cache.h"

class Proceso{
    public:
    string id;
    int memoriaSolicitada;
    int paginasAsignadas;
    SplayTree<int, Pagina> tablaPagina;
    Cache cacheLocal;

    Proceso():id(""), memoriaSolicitada(0), paginasAsignadas(0), cacheLocal(0){}
    Proceso(const string& idp, int mem, int tamPag): id(idp), memoriaSolicitada(mem), paginasAsignadas(0), cacheLocal(0) {}



    void actualizarCacheLocal(){
        int valor = max(1, (int)(paginasAsignadas*0.20));
        cacheLocal.setTamMax(valor);
    }

    void agregarPaginas(const Pagina& pagina){
        tablaPagina.insertar(pagina.paginaVirtual, pagina);
        paginasAsignadas++;
        actualizarCacheLocal();
    }

    bool eliminarPagina(int pagv){
        if(tablaPagina.eliminar(pagv)){
            paginasAsignadas--;
            actualizarCacheLocal();
            return true;
        }
        return false;
    }

    Pagina* buscarPagina(int pagv){
        return tablaPagina.buscar(pagv);
    }

    void mostrar() const{
        cout<< " Proceso:" << id << " Memoria Solicitada:" << memoriaSolicitada << " Paginas asignadas:" << paginasAsignadas << " Cache Local:" << cacheLocal.getTamMax() << " Usado:" << cacheLocal.tamActual() << endl;
    }
};