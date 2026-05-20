#pragma once

#include <iostream>
#include <functional>
#include <vector>

using namespace std;

template <typename K, typename V>
class SplayTree{
    private:
    struct Nodo{
        K llave;
        V valor;
        Nodo* izq;
        Nodo* der;

        Nodo(const K& k, const V& v) : llave(k), valor(v), izq(nullptr), der(nullptr) {}
    };

    Nodo* raiz;
    int cantidadNodos;
    

    Nodo* rotacionSimpleIzq(Nodo* x){
        Nodo* y = x->izq;
        x->izq = y->der;
        y->der = x;
        return y;
    }

    Nodo* rotacionSimpleDer(Nodo* x){
        Nodo* y = x->der;
        x->der = y->izq;
        y->izq = x;
        return y;
    }

    Nodo* splay(Nodo* raiz, const K& llave){
        if(!raiz || raiz->llave == llave) return raiz;
        if (llave < raiz->llave){
            if(!raiz->izq) return raiz;

            // izq, izq
            if(llave < raiz->izq->llave){
                raiz->izq->izq = splay(raiz->izq->izq, llave);
                raiz = rotacionSimpleIzq(raiz);
            }

            // izq, der
            if(llave > raiz->izq->llave){
                raiz->izq->der = splay(raiz->izq->der, llave);
                if(raiz->izq->der)
                    raiz->izq = rotacionSimpleDer(raiz->izq);
            }

            return raiz->izq ? rotacionSimpleIzq(raiz) : raiz;

        }
        else{
            if(!raiz->der) return raiz;

            // der,izq
            if(llave < raiz->der->llave){
                raiz->der->izq = splay(raiz->der->izq, llave);
                if(raiz->der->izq)
                    raiz->der = rotacionSimpleIzq(raiz->der);
            }

            // der, der
            else if(llave > raiz->der->llave){
                raiz->der->der = splay(raiz->der->der, llave);
                raiz = rotacionSimpleDer(raiz);
            }

            return raiz->der ? rotacionSimpleDer(raiz) : raiz;
        }
    }

    void limpiar(Nodo* n){
        if(!n) return;
        limpiar(n->izq);
        limpiar(n->der);
        delete n;
    }

    void recorrido(Nodo* n, function<void(const K&, const V&)> fn) const {
        if(!n) return;
        recorrido(n->izq, fn);
        fn(n->llave, n->valor);
        recorrido(n->der, fn);
    }

    void recolectarLlaves(Nodo* n, vector<K>& llaves) const{
        if(!n) return;
        recolectarLlaves(n->izq, llaves);
        llaves.push_back(n->llave);
        recolectarLlaves(n->der, llaves);
    }

    public:
    SplayTree() : raiz(nullptr), cantidadNodos() {}
    ~SplayTree(){
        limpiar(raiz);
    }

    void insertar(const K& llave, const V& valor){
        if(!raiz){ // no se encuentra la raiz, se crea
            raiz = new Nodo(llave, valor);
            cantidadNodos++;
            return;
        }
        raiz = splay(raiz, llave);
        if(raiz->llave == llave){ // se encuuetra la llave
            raiz->valor = valor; // cambiamos el valor del valor, lo actualizamos
            return;
        }

        Nodo* nuevoNodo = new Nodo(llave, valor); // no se encuentra la llave, se crea el nodo
        cantidadNodos++;
        if(llave < raiz->llave){ // si la llave del nodo nuevo es menor que la raiz, se va a la izquierda
            nuevoNodo->der = raiz;
            nuevoNodo->izq = raiz->izq;
            raiz->izq = nullptr;

        } else {
            nuevoNodo->izq = raiz;
            nuevoNodo->der = raiz->der;
            raiz->der = nullptr;
        }

        raiz = nuevoNodo;
    }

    bool eliminar(const K& llave){
        if(!raiz) return false;
        raiz = splay(raiz, llave);
        if(raiz->llave != llave) return false;
        cantidadNodos--;

        if(!raiz->izq) {
            Nodo* t = raiz;
            raiz = raiz->der;
            delete t;
        } else{
            Nodo* t = raiz;
            raiz = splay(raiz->izq, llave);
            raiz->der = t->der;
            delete t;
        }

        return true;
    }

    V* buscar(const K& llave){
        if(!raiz) return nullptr;
        raiz= splay(raiz,llave);
        if(raiz->llave == llave) return &raiz->valor;
        return nullptr;
    }

    bool contiene(const K& llave){
        return buscar(llave) != nullptr;
    }

    void hacerSplay(const K& llave){
        if(raiz) raiz = splay(raiz, llave);
    }

    void recorrido(function<void(const K&, const V&)>fn) const{
        recorrido(raiz, fn);
    }

    int tamanno() const{
        return cantidadNodos;
    }

    bool vacio() const{
        return raiz == nullptr;
    }

    vector<K> llaves() const{
        vector<K> resultado;
        recolectarLlaves(raiz, resultado);
        return resultado;
    }

    K llaveRaiz() const{
        if(raiz) return raiz->llave;
        throw runtime_error("Splay tree está vacío");
    }

};