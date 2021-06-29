#include "utilidades.h"


#include <fstream>
#include <sstream>
#include <cmath>

vector<unsigned int> readVectorUnsigned(string file, string keyword) {
    vector<unsigned int> params;
    ifstream myfile;
    myfile.open(file);

    char line[256];
    myfile.getline(line, 256);

    string strLine(line);
        
    while (strLine.compare(keyword) !=0) {
        myfile.getline(line, 256);
        strLine = line;
    }
    myfile.getline(line, 256);
    strLine = line;
    stringstream ss(strLine);

    int numParams;
    ss >> numParams;

    for (int i=0; i<numParams; i++) {
        myfile.getline(line, 256);
        strLine = line;
        stringstream ss2(strLine);
        unsigned int aux;
        ss2 >> aux;
        params.push_back(aux);
    }
    return params;
}

int readInt(string file, string keyword) {
    int param;
    ifstream myfile;
    myfile.open(file);

    char line[256];
    myfile.getline(line, 256);

    string strLine(line);
        
    while (strLine.compare(keyword) !=0) {
        myfile.getline(line, 256);
        strLine = line;
    }
    myfile.getline(line, 256);
    strLine = line;
    stringstream ss(strLine);
    
    ss >> param;

    return param;
}

double readDouble(string file, string keyword) {
    double param;
    ifstream myfile;
    myfile.open(file);

    char line[256];
    myfile.getline(line, 256);

    string strLine(line);
        
    while (strLine.compare(keyword) !=0) {
        myfile.getline(line, 256);
        strLine = line;
    }
    myfile.getline(line, 256);
    strLine = line;
    stringstream ss(strLine);
    
    ss >> param;

    return param;
}




Solucion getSolucionAleatoria(int dim) {
    Solucion s;
    int dimLibre = rand() % dim;
    vector<double> v(dim);
    for (int j=0; j<dim; j++) {
        
        if (j != dimLibre) {

            if (rand() % 2 )
                v[j] = 100;
            else 
                v[j] = -100;
        
        } else {
            // valor en [-100,100]
            v[j] = ( double(rand()) / (double(RAND_MAX)/200.0) ) - 100;

        }
    }
    s.v = v;
    return s;
}


vector<Solucion> getPoblacionInicial(int dim, int tamPoblacion) {
    vector<Solucion> p(tamPoblacion);

    // La poblacion inicial se genera en el borde del espacio de busqueda de forma aleatoria
    // Esto se hace poniendo todas las posiciones de la solucion igual a 100 o -100, menos una aleatoria que se saca de [-100, 100]
    for (int i=0; i<tamPoblacion; i++) {
        p[i] = getSolucionAleatoria(dim);
    }
    return p;
}


void moveInDirectionOf(Solucion &s, const Solucion &bestSol, double speed) {
    
    for (int i=0; i<s.v.size(); i++) {
        double disp = bestSol.v[i] - s.v[i];
        // Se mueve con una velocidad entre 0 y speed, y cada unidad mayor que uno sea su peso, se mueve 5*peso% menos.
        s.v[i] += speed * disp * ((61.0-s.peso)/60.0) * (rand() / double(RAND_MAX));
    }

    // Como ha cambiado, anulamos el fitness.
    s.fitness=-1;
}


double getDist(const vector<double> &v1, const vector<double> &v2) {
    double dist = 0;

    for (int i=0; i<v1.size(); i++) 
        dist += pow(v2[i]-v1[i], 2);
    
    return sqrt(dist);
}


double getDiversity(vector<Solucion> poblacion) {
    // ******************* Calculo de centroide *****************
    vector<double> centroide(poblacion[0].v.size(),  0);

    for ( int i=0; i<poblacion.size(); i++) {
        
        for ( int j=0; j<centroide.size(); j++) {  
            centroide[j] += poblacion[i].v[j];
        }
    }

    for ( int j=0; j<centroide.size(); j++)
        centroide[j] /= double(poblacion.size());

    // ******************* Calculo de la diversidad *****************

    double diferencia = 0;

    for (int i=0; i<poblacion.size(); i++) {
        double aux=0;
        for (int j=0; j<centroide.size(); j++) {
            aux += pow(poblacion[i].v[j] - centroide[j], 2);
        }
        diferencia += sqrt(aux);
    }
    diferencia /= poblacion.size();
    
    return diferencia;
}