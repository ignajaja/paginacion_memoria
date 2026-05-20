#pragma once

#include <iostream>
#include <string>
#include <chrono>

using namespace std;

class Pagina{
public:
    int paginaVirtual;
    int paginaFisica;
    string idProceso;
    bool valido;
    bool modificado;
    int cantidadReferencia;
    long long ultimaReferencia;

    Pagina (): paginaVirtual(-1), paginaFisica(-1), idProceso(""), valido(false), modificado(false), cantidadReferencia(0), ultimaReferencia(0) {}

    Pagina(int pv, int pf, string idp) : paginaVirtual(pv), paginaFisica(pf), idProceso(idp), valido(true), modificado(false), cantidadReferencia(0), ultimaReferencia(0) {}

    void touch() {
        cantidadReferencia++;
        ultimaReferencia = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    // int size() const{return }

    void mostrar() const;
};

void Pagina::mostrar() const{
    cout << " Pagina[pv=" << paginaVirtual << " pf=" << paginaFisica << " idP=" << idProceso << " valido=" << valido << " modificado=" << modificado << " referencias=" << cantidadReferencia << "]" << endl;
}