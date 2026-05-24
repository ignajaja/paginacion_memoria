#pragma once

#include <functional>
#include <stdexcept>

using namespace std;

// esta es una estructura de una lista custom que vamos a usar para guardar el caché

template <typename T>
class Lista{
    public:
    struct Nodo{
        // datos
        T dato;
        Nodo* ant;
        Nodo* sig;

        // constructor
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

    Nodo* insertarFrente(const T& v){ // conseguimos el valor por parámetro
        Nodo* n = new Nodo(v); // creamos un nodo con el valor
        n->sig = frente; // el siguiente de este nodo va a ser el frente, esto hace que sea el primero de todos los nodos
        n->ant = nullptr; // es el primero, no puede tener anterior
        if(frente) frente->ant = n; // si hay un frente, el anterior del frente (que antes era null) ahora es el nodo nuevo, por esto asignamos null al ant de frente
        frente = n; // pasa a ser el nuevo frente de la lista
        if(!fondo) fondo = n; // si no hay fondo es que la lista está vacía, el fondo también va a ser este nodo
        cantidad++; // auemntamos la cantidad de nodos en la lista
        return n;
    }

    void moverAlFrente(Nodo* n){
        if(n==frente) return; // si es el frente retorna
        if(n->ant) n->ant->sig = n->sig; // si tiene un anterior, el siguiente del anterior del nodo será el siguiente del nodo
        if(n->sig) n->sig->ant = n->ant; // si tiene un siguiente, el anterior del siguiente del nodo será el anterior del nodo
        if(n==fondo) fondo = n->ant; // si es el fondo, será el fondo del anterior

        n->ant = nullptr; // el anterior del primero es null
        n->sig = frente; // el siguiente va a ser el frente, ahora estamos de primeros
        if(frente) frente->ant = n; // lo convertimos en el anterior del frente
        frente = n; // y asignamos formalmente que este es el nuevo frente de la lista
    }

    bool eliminarFondo(T& valorEliminar){
        if(!fondo) return false; 
        valorEliminar = fondo->dato; // tomamos el valor del fondo
        Nodo* ultimo = fondo;
        fondo = fondo->ant;
        if(fondo) fondo->sig = nullptr; // asignamos null al siguiente del fondo
        else frente = nullptr; // si no hay fondo, no hay lista
        delete ultimo; // eliminamos
        cantidad--; // disminuímos cantidad
        return true;

    }

    void eliminarNodo(Nodo* n){
        if(!n) return;
        if(n->ant) n->ant->sig = n->sig; // si tiene un anterior, el siguiente del anterior del nodo será el siguiente del nodo
        else frente = n->sig; // sino la lista es solo él, frente va a ser el siguiente del nodo
        if(n->sig) n->sig->ant = n->ant; // si tiene un siguiente, el anterior del siguiente del nodo será el anterior del nodo
        else fondo = n->ant; // sino la lista es solo él, fondo va a ser el anterior del nodo
        delete n; // eliminamos el nodo 
        cantidad--; // disminuímos
    }

    void limpiar (){
        Nodo* actual = frente; // actual va a ser el frente
        while(actual){ // mientras no esté vacío
            Nodo* siguiente = actual->sig; // siguiente va a ser el siguiente del actual
            delete actual; // eliminamos el actual
            actual = siguiente; // el actual ahora es el siguiente y repetimos
        }
        frente = nullptr; // asignamos null a frente y fondo
        fondo = nullptr;
        cantidad = 0; // reset de la cantidad de nodos en la lista
    }

    void recorrer(function<void(const T&, int)> fn) const{
        Nodo* actual = frente; // actual cominenza en el frente
        int i = 0; // index, posición en la que estamos
        while (actual){ // mientras no esté vacío
            fn(actual->dato, i++); // function guarda el dato y posición, esto es lo que nos muestra 
            actual = actual->sig; // actual se vuelve el siguiente y repetimos
        }
    }
    
    // utilidades varias
    Nodo* getFrente() const {return frente;}
    Nodo* getFondo() const {return fondo;}
    int size() const{return cantidad;}
    bool vacio() const{return cantidad==0;}
    
};