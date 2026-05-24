#pragma once

#include <iostream>
#include <functional>
#include <vector>

using namespace std;

template <typename K, typename V>
class AVL{
private: // primero hacemos todas las funciones
    struct Nodo{ // hacemos la estructura de nodo para que sea lo que el avl tiene dentro
        K llave;
        V valor;
        Nodo* izq;
        Nodo* der;
        int altura;

        Nodo(const K& k, const V& v) : llave(k), valor(v), izq(nullptr), der(nullptr), altura(1) {} // constructor

    };

    // variables que vamos a necesitar
    Nodo* raiz; 
    int cantidadNodos;

    // en el archivo dice que debe llamarse altura, pero para que no hayan confunciones se ha llamado getAltura
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
        // lógica:
        // x va a ser el izquierdo de y, que nos lo dan por parámetro
        // t va a ser el derecho del izquierdo de y
        // ahora el derecho de x (el derecho del izquierdo de y) va a ser y
        // y el izquierdo de y va a ser t (que es el derecho del izquierdo de y)
        // al final se le pasa el hijo derecho de x a y, y pasa x a ser la raiz mientras y se vuelve su hijo izquierdo
        Nodo* x = y->izq;
        Nodo* t = x->der; // t es un temporario que nos va a ayudar a guardar un nodo mientras cambiamos las posiciones

        x->der = y;
        y->izq = t;

        actualizarAltura(y);
        actualizarAltura(x);

        return x;
    }

    Nodo* rotarIzquierda(Nodo* x){
        // mismo que rotarDerecha, pero con la lógica invertida
        Nodo* y = x->der;
        Nodo* t = y->izq; // t es un temporario que nos va a ayudar a guardar un nodo mientras cambiamos las posiciones

        y->izq = x;
        x->der = t;

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
            cantidadNodos++; // aumentamos la cantidad de nodos  y creamos uno nuevo
            return new Nodo(llave, valor);
        }

        if(llave < n->llave){ // si la llave que queremos ingresar es menor que la del nodo actual, lo comparamos con el de su izquierda 
            n->izq = insertar(n->izq, llave, valor);
        } else if(llave > n->llave){ // si la llave que queremos ingresar es mayor que la del nodo actual, lo comparamos con el de su derecha
            n->der = insertar(n->der, llave, valor);
        } else {
            n->valor = valor; // ingresamos el valor
            return n;
        }
        return balancear(n); // balanceamos para que el árbol siga siendo un avl
    }

    Nodo* nodoMenor(Nodo* n) const{ // buscamos el nodo menor, lo usamos para encontrar el menor nodo del subárbol derecho de la raiz
        Nodo* actual = n;
        while(actual->izq){
            actual = actual->izq; // nos vamos a la izquierda de todo
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
                Nodo* t = nodoMenor(n->der); // temporal que va a ser el menor nodo del subarbol derecho de la raiz
                n->llave = t->llave;
                n->valor = t->valor;
                bool enc = false; 
                n->der = eliminar(n->der, t->llave, enc);
                cantidadNodos++; 
            }

        }
        return balancear(n); // siempre balanceamos
    }

    Nodo* buscar(Nodo* n, const K& llave) const {
        if(!n) return nullptr; // si no existe se retorna null
        if(llave == n->llave) return n; // si se encuentra lo retornamos
        if(llave < n->llave) return buscar(n->izq, llave); // si la llave que buscamos es menor a la llave del nodo actual, se busca a su izquierda
        return buscar(n->der, llave); // de otro modo, se busca a su derecha, debido a que la única de las opciones que nos falta es que sea mayor

    }

    // function lo que hace es guardar cualquier tipo de dato dentro, como un contenedor univesal, cosa que no puede hacer una lista o un vector
    void inOrden(Nodo* n, function<void(const K&, const V&)> fn) const {
        if(!n) return;
        inOrden(n->izq, fn);
        fn(n->llave, n->valor);
        inOrden(n->der, fn);
    }

    void limpiar(Nodo* n){ // recorremos todo el árbol y eliminamos nodo a nodo
        if(!n) return;
        limpiar(n->izq);
        limpiar(n->der);
        delete n;
    }

    void recolectarLlaves(Nodo* n, vector<K>& llaves) const {
        if(!n) return;
        recolectarLlaves(n->izq, llaves);
        llaves.push_back(n->llave); // guardamos la llave en un vector
        recolectarLlaves(n->der, llaves);
    }

public:
    AVL() : raiz(nullptr), cantidadNodos(0) {}
    ~AVL(){
        limpiar(raiz); // para eliminarlo, lo limpiamos
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