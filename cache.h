#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include "pagina.h"
#include "lista.h"

struct entradaCache{
    string llave;
    Pagina pagina;

    entradaCache(): llave("") {}
    entradaCache(const string& k, const Pagina& p) : llave(k), pagina(p){}
};

class Cache{
    private:
    int tamMax;
    Lista<entradaCache> lista;

    unordered_map<string, Lista<entradaCache>::Nodo*> mapaCache;

    int hits;
    int miss;

    string crearLlave(const string& id, int pagv) const{
        return id + ": " + to_string(pagv);
    }

    public:
    Cache(int tm = 0): tamMax(tm), hits(0), miss(0) {}
    ~Cache() {}

    void setTamMax(int tam){
        tamMax = max(1,tam);
        while(lista.size() > tamMax){
            entradaCache evicted;
            lista.eliminarFondo(evicted);
            mapaCache.erase(evicted.llave);

        }
    }

    int getTamMax() const {return tamMax;}
    int tamActual() const {return lista.size();}
    bool lleno() const{return lista.size()>=tamMax;}
    bool vacio() const{return lista.vacio();}

    bool buscar(const string& idp, int pagv, Pagina& resultado){
        string llave = crearLlave(idp, pagv);
        auto it = mapaCache.find(llave);
        if(it == mapaCache.end()){ // si no lo encuentra cuenta miss
            miss++;
            return false;
        }
        lista.moverAlFrente(it->second); // si lo encuentra hace que el segundo sea el nuevo primero
        resultado = it->second->dato.pagina;
        hits++; // aumenta hits para estadísticas
        return true;
    }

    bool insertar(const string& idp, int pagv, const Pagina& pagina){
        if (tamMax <= 0) return false;
        string llave = crearLlave(idp, pagv);
        auto it = mapaCache.find(llave);

        if(it != mapaCache.end ()){
            it->second->dato.pagina = pagina;
            lista.moverAlFrente(it->second);
            return true;
        }
        if(lista.size() >= tamMax){
            entradaCache evicted;
            lista.eliminarFondo(evicted);
            mapaCache.erase(evicted.llave);
        }
        entradaCache entrada(llave, pagina);
        Lista<entradaCache>::Nodo* n = lista.insertarFrente(entrada);
        mapaCache[llave] = n;
        return true;
    }

    bool eliminar(const string& idp, int pagv){
        string llave = crearLlave(idp, pagv);
        auto it = mapaCache.find(llave);
        if(it == mapaCache.end()) return false;
        lista.eliminarNodo(it->second);
        mapaCache.erase(it);
        return true;
    }

    void procesoEliminar(const string& idp){
        Lista<entradaCache>::Nodo* actual = lista.getFrente();
        while(actual){
            Lista<entradaCache>::Nodo* sig = actual->sig;
            if(actual->dato.pagina.idProceso == idp){
                mapaCache.erase(actual->dato.llave);
                lista.eliminarNodo(actual);
            }
            actual = sig;
        }
    }

    void limpiar(){
        lista.limpiar();
        mapaCache.clear();
    }

    int getHits() const {return hits;}
    int getMiss() const {return miss;}

    void reiniciar() {
        hits = 0;
        miss = 0;
    }
    void mostrar(const string& nombre = "Cache") const{
        cout << " [" << nombre << "] tam:" << lista.size() << "/" << tamMax << endl << "    Hits:" << hits << " Misses" << miss << endl;
        lista.recorrer([](const entradaCache& e, int i){
            cout << "       " << i << " Id=" << e.pagina.idProceso << " PV=" << e.pagina.paginaVirtual << " PF=" << e.pagina.paginaFisica << endl;
        });
    }

};
