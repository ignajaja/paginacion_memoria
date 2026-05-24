#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include "pagina.h"
#include "lista.h"

struct entradaCache{ // esto es nuestro dato para el caché
    string llave;
    Pagina pagina;

    // constructores
    entradaCache(): llave("") {}
    entradaCache(const string& k, const Pagina& p) : llave(k), pagina(p){} 
};

class Cache{
    private:
    int tamMax;
    Lista<entradaCache> lista; // hacemos una lista con el caché

    unordered_map<string, Lista<entradaCache>::Nodo*> mapaCache; // usamos unordered map debodo a que es mucho más eficiente a la hora de buscar entre los datos que tiene dentro

    // contadores de los hits y misses en caché al buscar
    int hits;
    int miss;

    string crearLlave(const string& id, int pagv) const{
        return id + ": " + to_string(pagv); // creamos la llave de la entrada
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

    // utilidades varias
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
        string llave = crearLlave(idp, pagv); // creamos una llave nueva para el caché
        auto it = mapaCache.find(llave); // usamos el auto para asignar cualquier tipo de dato a it

        if(it != mapaCache.end ()){ // si no se encuetra en el mapa, se crea, esto lo hacemos para evitar repeticiones
            it->second->dato.pagina = pagina; // asignamos el valor de la página a el dato nuevo
            lista.moverAlFrente(it->second); // lo metemos de primero a la lista
            return true;
        }
        if(lista.size() >= tamMax){ // si está lleno, reemplazamos el del fondo de la lista
            entradaCache entradaEliminar; 
            lista.eliminarFondo(entradaEliminar); // eliminamos de la lista
            mapaCache.erase(entradaEliminar.llave); // eliminamos del mapa
        }
        entradaCache entrada(llave, pagina); // creamos la entrada
        Lista<entradaCache>::Nodo* n = lista.insertarFrente(entrada); // metemos al frente de la lista
        mapaCache[llave] = n; // metemos el dato en la posición de llave
        return true;
    }

    bool eliminar(const string& idp, int pagv){
        string llave = crearLlave(idp, pagv); // creamos la llave
        auto it = mapaCache.find(llave); // encontramos la llave
        if(it == mapaCache.end()) return false; // si no estó, no eliminamos nada
        lista.eliminarNodo(it->second); // la eliminamos de la lista
        mapaCache.erase(it); // la eliminamos del mapa
        return true;
    }

    void procesoEliminar(const string& idp){
        Lista<entradaCache>::Nodo* actual = lista.getFrente(); // asignamos un nodo actual que sea el primero de la lista
        while(actual){ // se repite hasta que se acabe la lista, esto hace que todos los items dentro queden una posición adelante después de eliminar el que buscamos
            Lista<entradaCache>::Nodo* sig = actual->sig; // asignamos un nodo que sea el siguiente en la lista
            if(actual->dato.pagina.idProceso == idp){ // si encontramos la id
                mapaCache.erase(actual->dato.llave); // elimnamos del mapa
                lista.eliminarNodo(actual); // eliminamos de la lista
            } 
            actual = sig; // vamos con el siguente
        }
    }

    void limpiar(){
        // limpiamos la lista y el mapa
        lista.limpiar();
        mapaCache.clear();
    }

    // utilidades par las estadísticas
    int getHits() const {return hits;}
    int getMiss() const {return miss;}

    void reiniciar() {
        // borramos las estadísticas
        hits = 0;
        miss = 0;
    }
    void mostrar(const string& nombre = "Cache") const{
        // mostramos todo lo que tengamos en el caché
        cout << " [" << nombre << "] tam:" << lista.size() << "/" << tamMax << endl << "    Hits:" << hits << " Misses" << miss << endl;
        lista.recorrer([](const entradaCache& e, int i){
            cout << "       " << i << " Id=" << e.pagina.idProceso << " PV=" << e.pagina.numeroPaginasVirtual << " PF=" << e.pagina.numeroPaginasFisica << endl;
        });
    }

};
