#pragma once

#include <string>
#include <iostream>
#include "pagina.h"
#include "splayTree.h"
#include "cache.h"

class Proceso{
    public:
    string idProceso;
    int memoriaSolicitada;
    int paginasAsignadas;
    Cache cacheLocal;
    SplayTree<int, Pagina> tablaDePagina;

    Proceso():idProceso(""), memoriaSolicitada(0), paginasAsignadas(0), cacheLocal(0){}
    Proceso(const string& idp, int mem, int tamPag): idProceso(idp), memoriaSolicitada(mem), paginasAsignadas(0), cacheLocal(0) {}



    void actualizarCacheLocal(){
        int valor = max(1, (int)(paginasAsignadas*0.20));
        cacheLocal.setTamMax(valor);
    }

    void agregarPaginas(const Pagina& pagina){
        tablaDePagina.insertar(pagina.numeroPaginasVirtual, pagina);
        paginasAsignadas++;
        actualizarCacheLocal();
    }

    bool eliminarPagina(int pagv){
        if(tablaDePagina.eliminar(pagv)){
            paginasAsignadas--;
            actualizarCacheLocal();
            return true;
        }
        return false;
    }

    Pagina* buscarPagina(int pagv){
        return tablaDePagina.buscar(pagv);
    }

    void mostrar() const{
        cout<< " Proceso:" << idProceso << " Memoria Solicitada:" << memoriaSolicitada << " Paginas asignadas:" << paginasAsignadas << " Cache Local:" << cacheLocal.getTamMax() << " Usado:" << cacheLocal.tamActual() << endl;
    }
};