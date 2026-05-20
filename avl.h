#pragma once

#include <iostream>
#include <functional>
#include <vector>

using namespace std;

template <typename K, typename V>
class AVL{
private: // primero hacemos todas las funciones
    struct Nodo{
        K llave;
        V valor;
        Nodo* izq;
        Nodo* der;
        int altura;

        Nodo(const K& k, const V& v) : llave(k), valor(v), izq(nullptr), der(nullptr), altura(1) {}

    };

    Nodo* raiz;
    int cantidadNodos;

    int getAltura(Nodo* n){
        return n ? n->altura : 0; // null si no existe el nodo, de otra manera da la altura del mismo

    }

    int factorBalance(Nodo* n){
        return n ? getAltura(n->izq) - getAltura(n->der) : 0; // 0 si no hay nodo, sino es la altura de el de la izquierda, que debería ser el mayor, menos el de la derecha
    }

    void actualizarAltura(Nodo* n){
        if(n){
            n->altura = 1 + max(getAltura(n->izq), getAltura(n->der)); // actualizamos la altura de cada nodo sacando el maximo entre su derecha e izquierda
        }
    }

    Nodo* rotarDerecha(Nodo* y){
        Nodo* x = y->izq;
        Nodo* t = x->der; // t es un temporario que nos va a ayudar a guardar un nodo mientras cambiamos las posiciones

        x->der = y;
        y->izq = t;

        actualizarAltura(y);
        actualizarAltura(x);

        return x;
    }

    Nodo* rotarIzquierda(Nodo* x){
        Nodo* y = x->izq;
        Nodo* t = y->der; // t es un temporario que nos va a ayudar a guardar un nodo mientras cambiamos las posiciones

        y->der = x;
        x->izq = t;

        actualizarAltura(x);
        actualizarAltura(y);

        return y;
    }

    Nodo* balancear(Nodo* n){
        actualizarAltura(n);
        int factor = factorBalance(n);
        // caso izq, izq
        if(factor > 1 && factorBalance(n->izq) >= 0){
            return rotarDerecha(n);
        }
        // caso izq, der
        if(factor > 1 && factorBalance(n->izq) < 0){
            n->izq = rotarIzquierda(n->izq);
            return rotarDerecha(n);
        }
        // caso der, izq
        if(factor < -1 && factorBalance(n->der) <= 0){
            return rotarIzquierda(n);
        }
        // caso der, der
        if(factor < -1 && factorBalance(n->der) > 0){
            n->der = rotarDerecha(n->der);
            return rotarIzquierda(n);
        }
        return n;
    }

    Nodo* insertar(Nodo* n, const K& llave, const V& valor){
        if(n == nullptr){
            cantidadNodos++;
            return new Nodo(llave, valor);
        }

        if(llave < n->llave){
            n->izq = insertar(n->izq, llave, valor);
        } else if(llave > n->llave){
            n->der = insertar(n->der, llave, valor);
        } else {
            n->valor = valor;
            return n;
        }
        return balancear(n);
    }

    Nodo* nodoMenor(Nodo* n) const{
        Nodo* actual = n;
        while(actual->izq){
            actual = actual->izq;
        }
        return actual;
    }

    Nodo* eliminar(Nodo* n, const K& llave, bool& encontrado){
        if(n == nullptr) return nullptr;

        if(llave < n->llave){ // si lo comparamos con uno mayor que el que ingresamos busca a su izquierda
            n->izq = eliminar(n->izq, llave, encontrado);
        } else if(llave > n->llave){ // si lo comparamos con uno menor que el que ingresamos, busca a su derecha
            n->der = eliminar(n->der, llave, encontrado);
        } else { // lo encontramos
            encontrado = true;
            cantidadNodos--;

            if(!n->izq || !n->der){ // si no tiene hijos
                Nodo* t = n->izq ? n->izq : n->der;
                if(!t){ // si no hay dato a la izquierda
                    delete n;
                    return nullptr;
                } else{ // si hay un dato a la izquierda
                    *n = *t;
                    delete t;
                }
            } else {
                Nodo* t = nodoMenor(n->der);
                n->llave = t->llave;
                n->valor = t->valor;
                bool enc = false;
                n->der = eliminar(n->der, t->llave, enc);
                cantidadNodos++;
            }

        }
        return balancear(n);
    }

    Nodo* buscar(Nodo* n, const K& llave) const {
        if(!n) return nullptr;
        if(llave == n->llave) return n;
        if(llave < n->llave) return buscar(n->izq, llave);
        return buscar(n->der, llave);

    }

    void inOrden(Nodo* n, function<void(const K&, const V&)> fn) const {
        if(!n) return;
        inOrden(n->izq, fn);
        fn(n->llave, n->valor);
        inOrden(n->der, fn);
    }

    void limpiar(Nodo* n){
        if(!n) return;
        limpiar(n->izq);
        limpiar(n->der);
        delete n;
    }

    void recolectarLlaves(Nodo* n, vector<K>* llaves) const {
        if(!n) return;
        recolectarLlaves(n->izq, llaves);
        llaves.push_back(n->llave);
        recolectarLlaves(n->der, llaves);
    }

public:
    AVL() : raiz(nullptr), cantidadNodos(0) {}
    ~AVL(){
        limpiar(raiz);
    }

    void insertar(const K& llave, const V& valor){
        raiz = insertar(raiz, llave, valor);
    }
    
    bool eliminar(const K& llave){
        bool encontrado = false;
        raiz = eliminar(raiz, llave, encontrado);
        return encontrado;

    }

    V* buscar(const K& llave){
        Nodo* n = buscar(raiz, llave);
        return n ? &n->valor : nullptr;

    }

    const V* buscar(const K& llave) const{
        Nodo* n = buscar(raiz, llave);
        return n ? &n->valor : nullptr;
    }

    bool contiene(const K& llave) const{
        return buscar(raiz, llave) != nullptr;
    }

    void inOrden(function<void(const K&, const V&)> fn) const{
        inOrden(raiz, fn);
    }

    int getAltura() const{
        return getAltura(raiz);
    }

    int size() const{
        return cantidadNodos;
    }
    
    bool vacio() const{
        return raiz == nullptr;
    }

    bool minimo(K& llave, V& valor) const{
        if(!raiz) return false;
        Nodo* n = nodoMenor(raiz);
        llave = n->llave;
        valor = n->valor;
        return true;
    }

    vector<K> llaves() const{
        vector<K> resultado;
        recolectarLlaves(raiz, resultado);
        return resultado;
    }

};