#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <vector>
#include <string>
#include <iostream>
#include <random>

#include "Solucion.h"

using namespace std;

/**
 * @brief Lee un vector de enteros positivos desde el fichero file, seguido de la palabra keyword.
 * @param file Ruta del fichero de parametros.
 * @param keyword palabra que especifica que parte del fichero hay que leer.
 * @return vector<unsigned int> Vector con los parametros leidos.
 */
vector<unsigned int> readVectorUnsigned(string file, string keyword);

/**
 * @brief Lee un entero desde el fichero file, seguido de la palabra keyword.
 * @param file Ruta del fichero de parametros.
 * @param keyword palabra que especifica que parte del fichero hay que leer.
 * @return un int con el paramtero.
 */
int readInt(string file, string keyword);

/**
 * @brief Lee un double desde el fichero file, seguido de la palabra keyword.
 * @param file Ruta del fichero de parametros.
 * @param keyword palabra que especifica que parte del fichero hay que leer.
 * @return un double con el paramtero.
 */
double readDouble(string file, string keyword);



/**
 * @brief Genera una solucion aleatoria en la forntera del espacio de busqueda.
 * @param dim la dimension de las soluciones
 * @return Solucion 
 */
Solucion getSolucionAleatoria(int dim);


/**
 * @brief Genera una poblacion inicial de tamaño tamPoblacion, con soluciones de dimension dim 
 * 
 * @return vector<vector<double>> Vector de soluciones iniciales
 */
vector<Solucion> getPoblacionInicial(int dim, int tamPoblacion);


/**
 * @brief Mueve la solucion s hacia la solucion bestSol teniendo en cuenta la velocidad.
 * @param s Solucion a mover.
 * @param bestSol Solucion hacia donde se mueve s.
 * @param speed Velocidad que se quiere usar para mover la solucion.
 */
void moveInDirectionOf(Solucion &s, const Solucion &bestSol, double speed);

/**
 * @brief Obtiene la distancia euclidea entre dos vectores de doubles v1 y v2.
 * @param v1 Uno de los vectores que se quiere calcular su distancia.
 * @param v2 Uno de los vectores que se quiere calcular su distancia.
 * @return double La distancia entre v1 y v2.
 */
double getDist(const vector<double> &v1, const vector<double> &v2);

/**
 * @brief Calcula la distancia media de cada solucion con el centroide de la poblacion.
 * @param poblacion La poblacion que queremos obtener su diversidad.
 * @return double El valor que usaremos para medir diversidad.
 */
double getDiversity(vector<Solucion> poblacion);

#endif //UTILIDADES_H