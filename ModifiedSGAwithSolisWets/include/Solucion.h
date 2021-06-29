#ifndef _SOLUCION
#define _SOLUCION

#include <vector>


using namespace std;


struct Solucion {
        // Vector con los valores de la solucion para cada dimension.
        vector<double> v;

        // Valor de la funcion objetivo de la solucion actual.
        double fitness=-1;

        // El peso de la solucion. Influira en la velocidad de su movimiento.
        int peso = 1;
};



#endif