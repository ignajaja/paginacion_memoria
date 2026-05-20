#pragma once

#include <functional>
#include <stdexcept>

using namespace std;

template <typename T>
class Lista{
    public:
    struct Nodo{
        T dato;
        Nodo* ant;
        Nodo* sig;

        Nodo(const T& dat): dato(dat), ant(nullptr), sig(nullptr) {}
    };

    private:
    Nodo* frente;
    Nodo* fondo;
    int cantidad;

    public:
    Lista() : frente(nullptr), fondo(nullptr), cantidad(0) {}
    ~Lista(){
        limpiar();
    }

    Nodo* insertarFrente(const T& v){
        Nodo* n = new Nodo(v);
        n->sig = frente;
        n->ant = nullptr;
        if(frente) frente->ant = n;
        frente = n;
        if(!fondo) fondo = n;
        cantidad++;
        return n;
    }

    void moverAlFrente(Nodo* n){
        if(n==frente) return;
        if(n->ant) n->ant->sig = n->sig;
        if(n->sig) n->sig->ant = n->ant;
        if(n==fondo) fondo = n->ant;

        n->ant = nullptr;
        n->sig = frente;
        if(frente) frente->ant = n;
        frente = n;
    }

    bool eliminarFondo(T& valorEliminado){
        if(!fondo) return false;
        valorEliminado = fondo->dato;
        Nodo* ultimo = fondo;
        fondo = fondo->ant;
        if(fondo) fondo->sig = nullptr;
        else frente = nullptr;
        delete ultimo;
        cantidad--;
        return true;

    }

    void eliminarNodo(Nodo* n){
        if(!n) return;
        if(n->ant) n->ant->sig = n->sig;
        else frente = n->sig;
        if(n->sig) n->sig->ant = n->ant;
        else fondo = n->ant;
        delete n;
        cantidad--;
    }

    void limpiar (){
        Nodo* actual = frente;
        while(actual){
            Nodo* siguiente = actual->sig;
            delete actual;
            actual = siguiente;
        }
        frente = nullptr;
        fondo = nullptr;
        cantidad = 0;
    }

    void recorrer(function<void(const T&, int)> fn) const{
        Nodo* actual = frente;
        int i = 0;
        while (actual){
            fn(actual->dato, i++);
            actual = actual->sig;
        }
    }
    
    Nodo* getFrente() const {return frente;}
    Nodo* getFondo() const {return fondo;}
    int size() const{return cantidad;}
    bool vacio() const{return cantidad==0;}
    
};