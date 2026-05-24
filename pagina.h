#pragma once

#include <iostream>
#include <string>
#include <chrono>

using namespace std;

class Pagina{
public:
    int numeroPaginasVirtual;
    int numeroPaginasFisica;
    string idProceso;
    bool bitValida;
    bool bitModificada;
    int cantidadReferencias;
    long long ultimaReferencia;

    Pagina (): numeroPaginasVirtual(-1), numeroPaginasFisica(-1), idProceso(""), bitValida(false), bitModificada(false), cantidadReferencias(0), ultimaReferencia(0) {}

    Pagina(int pv, int pf, string idp) : numeroPaginasVirtual(pv), numeroPaginasFisica(pf), idProceso(idp), bitValida(true), bitModificada(false), cantidadReferencias(0), ultimaReferencia(0) {}

    void touch() {
        cantidadReferencias++;
        ultimaReferencia = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
    }

    // int size() const{return }

    void mostrar() const;
};

void Pagina::mostrar() const{
    cout << " Pagina[pv=" << numeroPaginasVirtual << " pf=" << numeroPaginasFisica << " idP=" << idProceso << " bitValida=" << bitValida << " bitModificada=" << bitModificada << " referencias=" << cantidadReferencias << "]" << endl;
}