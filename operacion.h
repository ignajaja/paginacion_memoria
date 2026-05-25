#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

#include "memoria.h"

class Operacion{
    private:
    Memoria* mem;

    vector<string> hacerToken(const string& linea){
        vector<string> tokens;
        istringstream iss(linea);
        string token;
        while(iss >> token) tokens.push_back(token);
        return tokens;
    }

    bool parseExecute(const string& linea){
        if(linea.empty())return true; // si está limpia
        auto tokens = hacerToken(linea);
        if(tokens.empty()) return true;
        const string& operacion = tokens[0];
        if(operacion == "MEMORIA_TOTAL" || operacion == "TAMANO_PAGINA" || operacion == "TAMANO_CACHE_GLOBAL"){
            return true;
        }

        if(operacion == "NEW_PROCESS"){
            if(tokens.size() < 3){
                cout << "! Error, para hacer un NEW_PROCESS debe escribirse \"NEW_PROCESS <idProceso> <memoria>\"" << endl;
                return false;
            }
            return mem->nuevoProceso(tokens[1], stoi(tokens[2]));

        }
        if(operacion == "END_PROCESS"){
            if(tokens.size() < 2){
                cout << "! Error, para hacer un END_PROCES" << endl;
                return false;
            }
            return mem->terminarProceso(tokens[1]);
        }

        if(operacion == "ACCESS"){
            if(tokens.size() < 3){
                cout << "! Error, para hacer un ACCESS debe escribirse \"ACCESS <idProceso> <direcccion>\"" << endl;
                return false;
            }
            return mem->acceso(tokens[1], stoi(tokens[2]));
        }
        if(operacion == "ALLOCATE"){
            if(tokens.size() < 3){
                cout << "! Error, para hacer un ALLOCATE debe escribirse \"ALLOCATE <idProceso> <memoria>\"" << endl;
                return false;
            }
            return mem->asignar(tokens[1], stoi(tokens[2]));
        }
        if(operacion == "FREE"){
            if(tokens.size() < 4){
                cout << "! Error, para hacer un FREE debe escribirse \"FREE <idProceso> <dirección> <bytes>\"" << endl;
                return false;
            }
            return mem->memoriaLibre(tokens[1], stoi(tokens[2]), stoi(tokens[3]));
        }
        if(operacion == "PRINT_STATE"){
            mem->printState();
            return true;
        }
        if(operacion == "PRINT_METRICS"){
            mem->printMetricas();
            return true;
        }
        if(operacion == "PRINT_PROCESS"){
            if(tokens.size() < 2){
                cout << "! Error, para hacer un PRINT_PROCESS debe escribirse \"PRINT_PROCESS <idProceso>\"" << endl;
                return false;
            }
            mem->printProceso(tokens[1]);
            return true;
        }
        cout << "! Error, opción desconocida" << endl;
        return false;

    }
    bool leerConfig(istream& stream, int& memTotal, int& tamPag, int& cacheGlobal, vector<string>& lineasRestantes){
        memTotal = 0;
        tamPag = 0;
        cacheGlobal = 0;
        string linea;
        while(getline(stream, linea)){
            if(linea.empty()){
                continue; // si está en blanco se ignora
            }
            auto tokens = hacerToken(linea);
            if(tokens.empty()){
                continue;
            }
            if(tokens[0] == "MEMORIA_TOTAL" && tokens.size() >= 2){
                memTotal = stoi(tokens[1]);
            } else if (tokens[0] == "TAMANO_PAGINA" && tokens.size() >=2){
                tamPag = stoi(tokens[1]);
            } else if (tokens[0] == "TAMANO_CACHE_GLOBAL" && tokens.size() >= 2){
                cacheGlobal = stoi(tokens[1]);
            } else {
                lineasRestantes.push_back(linea);
            }
            if(memTotal > 0 && tamPag > 0 &&(tokens[0] != "MEMORIA_TOTAL" && tokens[0] != "TAMANO_PAGINA" && tokens[0] != "TAMANO_CACHE_GLOBAL"))
                ; // esto lo hacemos para que siga leyendo el while
        }
        return memTotal > 0 && tamPag > 0;
    }


    public:
    Operacion(): mem(nullptr) {}
    ~Operacion(){delete mem;}

    bool runFile(const string& nombre){
        ifstream archivo(nombre);
        if(!archivo.is_open()){
            cout << "! Error, no se pudo abrir el archivo " << nombre << endl;
            throw runtime_error(""); // hacemos un throw para que lo atrape el catch del main
            return false;
        }
        int memTotal = 0;
        int tamPag = 0;
        int cacheGlobal = 0;
        string linea;
        vector<string> operaciones;

        bool finalizado = false;
        
        while(getline(archivo, linea)){
            if(linea.empty()) continue;
            auto tokens = hacerToken(linea);
            if(tokens.empty()) continue;

            if(tokens[0] == "MEMORIA_TOTAL" && tokens.size() >= 2)
                memTotal = stoi(tokens[1]);
            else if(tokens[0] == "TAMANO_PAGINA" && tokens.size() >= 2)
                tamPag = stoi(tokens[1]);
            else if(tokens[0] == "TAMANO_CACHE_GLOBAL" && tokens.size() >= 2)
                cacheGlobal = stoi(tokens[1]);
            else
                operaciones.push_back(linea);
        }
        archivo.close();

        if(memTotal <= 0 || tamPag <= 0){
            cout << "! Error, parámetros de configuración no están completos" << endl;
            return true;
        }

        if(memTotal % tamPag != 0){
            cout << "La memoria no es múltiplo del tamaño de página, se usará " << memTotal / tamPag << " páginas" << endl;
        }
        mem = new Memoria(memTotal, tamPag, cacheGlobal);

        for (const auto& op : operaciones){
            try {
                parseExecute(op);
            } catch (const exception& e) {
                cout << "! Error ejecutando operación: \"" << op << "\" -> " << e.what() << endl;
            }
        }

        return true;
    }

    // ahora el modo interactivo
    void runInteractiveMode(){
        cout << "Modo interactivo" << endl;

        int memTotal = 0;
        int tamPag = 0;
        int cacheGlobal = 0;
        
        cout << "Memoria total: ";
        cin >> memTotal;
        cout << "Tamaño de cada página: ";
        cin >> tamPag;
        cout << "Tamaño de caché global: ";
        cin >> cacheGlobal;
        cin.ignore();

        if(memTotal <= 0 || tamPag <= 0){
            cout << "! Error, ingrese parámetros válidos" << endl;
            return;
        }

        mem = new Memoria(memTotal, tamPag, cacheGlobal);
        
        string linea;
        while(true){

        cout << "Comandos: " << endl; 
        cout << " - NEW_PROCESS <idProceso> <memoria>" << endl;
        cout << " - END_PROCESS <idProceso>" << endl;
        cout << " - ACCESS <idProceso> <dirección>" << endl;
        cout << " - ALLOCATE <idProceso> <memoria>" << endl;
        cout << " - FREE <idProceso> <dirección inicial> <bytes>" << endl;
        cout << " - PRINT_STATE" << endl;
        cout << " - PRINT_METRICS" << endl;
        cout << " - PRINT_PROCESS" << endl;
        cout << " - EXIT" << endl;

            cout << ":";
            if(!getline(cin,linea)) break;
            if(linea == "EXIT"){
                break;
            }
            parseExecute(linea);
        }
        mem->printMetricas();
    }
};