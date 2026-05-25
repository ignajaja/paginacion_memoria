#pragma once

#include <string>
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include "avl.h"
#include "splayTree.h"
#include "cache.h"
#include "proceso.h"
#include "pagina.h"

struct Metricas{
    int accesosTotales = 0; // total de accesos a memoria
    // estadísticas de misses y hits
    int hitsLocal = 0;
    int missLocal = 0;
    int hitsGlobal = 0;
    int missGlobal = 0;
};

class Memoria{
    private:
    int memoriaTotal;
    int tamPagina;
    int totalPaginas;
    int tamCacheGlobal;

    AVL<int, Pagina> paginasVacias;
    AVL<int, Pagina> paginasUsadas;
    unordered_map<string, Proceso> procesos;
    Cache cacheGlobal;

    Metricas metricas;

    bool asignarPaginas(Proceso& proc, int cantidad, int inicio = -1);
    int proximaPaginav(const Proceso& proc) const;

    public:
    Memoria(int memTotal, int tamPag, int tamCacheG);
    bool nuevoProceso(const string& idp, int memoria);
    bool terminarProceso(const string& idp);
    bool acceso(const string& idp, int address);
    bool asignar(const string& idp, int memoriaExtra);
    bool memoriaLibre(const string& idp, int iniciarAddress, int bytes);

    void printState() const;
    void printMetricas() const;
    void printProceso(const string& idp) const;

    int getTotalPaginas() const{return totalPaginas;}
    int getTotalPaginasVacias() const{return paginasVacias.size();}
    int getTotalPaginasUsadas() const{return paginasUsadas.size();}

};

Memoria::Memoria(int memTotal, int tPagina, int tCacheGlobal): memoriaTotal(memTotal), tamPagina(tPagina), tamCacheGlobal(tCacheGlobal), cacheGlobal(tCacheGlobal){
    totalPaginas = memoriaTotal / tamPagina; // hacemos el total de páginas que poseemos
    for (int i = 0; i < totalPaginas; i++){
        Pagina p;
        p.numeroPaginasFisica = i; // hacemos las páginas físicas
        p.bitValida = false;
        paginasVacias.insertar(i, p);
    }

    cout << "Iniciando memoria paginada..." << endl << "    Memoria total:" << memoriaTotal << endl << "    Tamaño de página:" << tamPagina << endl << "    Páginas totales:" << totalPaginas << endl << "    Cache global:" << tamCacheGlobal << endl;


}

int Memoria::proximaPaginav(const Proceso& proc) const{
    auto llaves = proc.tablaDePagina.llaves(); // conseguimos las llaves del mapa
    if(llaves.empty()) return 0; // si no hay llaves el mapa está vacío
    return llaves.back() + 1; // retornamos la última llave más 1, para hacer que el índice sea uno después que el último
}

bool Memoria::asignarPaginas(Proceso& proc, int cantidad, int iniciarVirt){
    if(paginasVacias.size() <  cantidad){
        cout << "! Error, páginas llenas" << endl;
        return false;
    }

    int paginaVirtualBase = (iniciarVirt >= 0) ? iniciarVirt : proximaPaginav(proc); // 
    for(int i = 0; i < cantidad; i++){
        int llaveFisica;
        Pagina paginaFisica;
        paginasVacias.minimo(llaveFisica, paginaFisica);
        paginasVacias.eliminar(llaveFisica);

        int paginaVirtual = paginaVirtualBase + i;
        Pagina nueva(paginaVirtual, llaveFisica, proc.idProceso);
        nueva.touch();

        paginasUsadas.insertar(llaveFisica, nueva);
        proc.agregarPaginas(nueva);
    }
    return true;
}

bool Memoria::nuevoProceso(const string& idp, int memoria){
    cout << "Nuevo proceso: " << idp << endl;
    if(procesos.count(idp)){
        cout << "! Error, ya hay un proceso con esta id" << endl;
        return false;
    }

    int pagNecesarias = (memoria + tamPagina -1) / tamPagina;

    if(paginasVacias.size() < pagNecesarias){
        cout << "! Error, no hay suficiente memoria, por " << pagNecesarias << " páginas" << endl;
        return false;
    }

    Proceso proc(idp, memoria, tamPagina);
    procesos[idp] = proc;

    if(!asignarPaginas(procesos[idp], pagNecesarias)){
        procesos.erase(idp);
        return false;
    }

    cout << "Éxito" << endl << "Proceso: " << idp <<  "  Páginas:" << pagNecesarias << endl;
    return true;
}

bool Memoria::terminarProceso(const string& idp){
    cout << "Terminar proceso:" << idp << endl;
    if(!procesos.count(idp)){
        cout << "! Error, este proceso no existe" << endl;
        return false;
    }
    Proceso& proc = procesos[idp];

    auto llaves = proc.tablaDePagina.llaves();
    for (int paginaVirtual: llaves){
        Pagina* p= proc.tablaDePagina.buscar(paginaVirtual);
        if(p){
            int pFisica = p->numeroPaginasFisica;
            paginasUsadas.eliminar(pFisica);
            Pagina pagVacia;
            pagVacia.numeroPaginasFisica = pFisica;
            pagVacia.bitValida = false;
            paginasVacias.insertar(pFisica, pagVacia);
        }
    }
    cacheGlobal.procesoEliminar(idp);
    procesos.erase(idp);
    cout << "Eliminado con éxito: " << idp << endl;
    return true;
}

bool Memoria::acceso(const string& idp, int address){
    cout << "Acceso a: " << idp << endl;
    if(!procesos.count(idp)){
        cout << "! Error, no se encontró" << endl;
        return false;
    }

    if(address < 0){
        cout << "! Error, dirección inválida" << endl;
        return false;
    }

    int paginaVirtual = address / tamPagina;
    int offset = address % tamPagina;

    cout << "Página virtual: " << paginaVirtual << ". Desplazamiento: " << offset << endl;

    Proceso& proc = procesos[idp];
    metricas.accesosTotales++;

    Pagina* pagProceso = proc.tablaDePagina.buscar(paginaVirtual);
    if(!pagProceso){
        cout << "! Error, no está asignada al proceso" << endl;
        return false;
    }

    Pagina pagEncontrada = *pagProceso;
    Pagina pagCache;
    bool inLocal = proc.cacheLocal.buscar(idp, paginaVirtual, pagCache);

    if(inLocal){
        metricas.hitsLocal++;
        cout << "hit en caché local" << endl;
        proc.tablaDePagina.hacerSplay(paginaVirtual);
        pagProceso->touch();
        proc.cacheLocal.insertar(idp, paginaVirtual, *pagProceso);
        cout << endl;
        return true;

    }

    metricas.missLocal++;
    cout << "miss en caché local" << endl;

    bool inGlobal = cacheGlobal.buscar(idp, paginaVirtual, *pagProceso);

    if(inGlobal){
        metricas.hitsGlobal++;
        cout << "hit en caché global" << endl;
        pagProceso->touch();
        proc.cacheLocal.insertar(idp, paginaVirtual, *pagProceso);
        proc.tablaDePagina.hacerSplay(paginaVirtual);
        cout << endl;
        return true;
    }

    metricas.missGlobal++;
    cout << "miss en caché global" << endl;
    pagProceso->touch();
    proc.tablaDePagina.hacerSplay(paginaVirtual);
    proc.cacheLocal.insertar(idp, paginaVirtual, *pagProceso);
    cacheGlobal.insertar(idp, paginaVirtual, *pagProceso);
    cout << endl;
    return true;

}

bool Memoria::asignar(const string& idp, int memoriaExtra){
    cout << "Asignar memoria extra" << endl;
    if(!procesos.count(idp)){
        cout << "! Error, no existe el proceso" << endl;
        return false;
    }
    if(memoriaExtra <= 0){
        cout << "Tamaño de memoria inválido" << endl;
        return false;
    }
    int pagNecesarias = (memoriaExtra + tamPagina -1) /tamPagina;

    if(paginasVacias.size() < pagNecesarias){
        cout << "! Error, memoria insuficiente" << endl;
        return false;
    }

    Proceso& proc = procesos[idp];
    int pagPrevias = proc.paginasAsignadas;

    if(!asignarPaginas(proc,pagNecesarias)){
        return false;
    }
    cout << pagNecesarias << "Páginas adicionales" << endl;
    return true;

}

bool Memoria::memoriaLibre(const string& idp, int inicioAddress, int bytes) {
    cout << "Memoria libre: " << idp << ". " << bytes << " bytes" << endl;
    if(!procesos.count(idp)){
        cout << "! Error, el proceso no existe" << endl;
        return false;
    }
    if(inicioAddress < 0 || bytes <= 0){
        cout << "Parametros inválidos" << endl;
        return false;
    }
    int inicioPaginaVirtual = inicioAddress / tamPagina;
    int finalPaginaVirtual = (inicioAddress + bytes -1) / tamPagina;

    Proceso& proc = procesos[idp];
    int liberado = 0;

    for(int paginaV = inicioPaginaVirtual; paginaV <= finalPaginaVirtual; paginaV++){
        Pagina* p = proc.tablaDePagina.buscar(paginaV);
        if(!p){
            cout << "* Página virtual no asignada" << endl;
            continue;
        }
        int paginaF = p->numeroPaginasFisica;
        proc.cacheLocal.eliminar(idp, paginaV);
        cacheGlobal.eliminar(idp, paginaV);

        paginasUsadas.eliminar(paginaF);
        Pagina paginaLibre;
        paginaLibre.numeroPaginasFisica = paginaF;
        paginaLibre.bitValida = false;
        paginasVacias.insertar(paginaF, paginaLibre);

        proc.eliminarPagina(paginaV);
        liberado++;

    }

    if(liberado == 0){
        cout << "No se ha liberado ninguna página" << endl;
        return false;
    }
    cout << "Páginas liberadas: " << liberado << endl;
    return true;
}

void Memoria::printState() const{
    cout << endl << "Estado del sistema" << endl;
    cout << "   Páginas totales: " << totalPaginas << endl;
    cout << "   Páginas libres: " << paginasVacias.size() << endl;
    cout << "   Páginas usadas: " << paginasUsadas.size() << endl;
    cout << "   Procesos activos: " << procesos.size() << endl;
}

void Memoria::printMetricas() const{
    cout << endl << " Estado de las métricas" << endl;
    cout << "   Accesos totales: " << metricas.accesosTotales << endl;
    cout << "   Hits caché local: " << metricas.hitsLocal << endl;
    cout << "   Misses caché local: " << metricas.missLocal << endl;
    cout << "   Hits caché global: " << metricas.hitsGlobal << endl;
    cout << "   Misses caché global: " << metricas.missGlobal << endl;
    cout << "   Páginas asignadas: " << paginasUsadas.size() << endl;
    cout << "   Páginas libres: " << paginasVacias.size() << endl;

}

void Memoria::printProceso(const string& idp)const {
    auto it = procesos.find(idp);
    if(it == procesos.end()){
        cout << "! Error, no existe el proceso" << endl;
        return;
    }
    it->second.mostrar();
    it->second.cacheLocal.mostrar("CacheLocal-" + idp);

    cout << "Tabla de páginas" << endl;
    it->second.tablaDePagina.recorrido([](const int& pv, const Pagina& p){
        cout << "   pág vir=" << pv << "> pág fís=" << p.numeroPaginasFisica << ". referencias = " << p.cantidadReferencias << endl;
    });


}