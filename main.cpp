#include <iostream>
#include <string>
#include "operacion.h"

using namespace std;

int main(int argc, char* argv []){
    Operacion operacion;
    if(argc >= 2){
        string nombre = argv[1];
        cout << "Cargando operaciones de: " << nombre << endl;
        operacion.runFile(nombre);
    } else {
        operacion.runInteractiveMode();
    }

    return 0;
}