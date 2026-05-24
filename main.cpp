#include <iostream>
#include <string>
#include "operacion.h"

using namespace std;

int main(int argc, char* argv []){
    Operacion operacion;

    try{
        cout << "Corriendo modo archivo" << endl;
        string nombre = "input.txt"; // nombre fijo del archivo que tiene lo que queremos hacer
        cout << "Cargando operaciones de: " << nombre << endl;
        operacion.runFile(nombre); // lanzamos el runFile
    } catch (const runtime_error& e) {
        cout << "Corriendo modo interactivo" << endl;
        operacion.runInteractiveMode(); // en caso que el modo archivo no corra, usamos modo interactivo
    }

    return 0;
}