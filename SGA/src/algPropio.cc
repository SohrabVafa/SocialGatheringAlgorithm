extern "C"
{
#include "cec17.h"
}

#include "utilidades.h"
#include "unix_timer.h"
#include "Solucion.h"
#include <algorithm>    // std::random_shuffle
#include <fstream>

using namespace std;


void algPropio(const int funcid, const int dim, const int tamPoblacion, const int tamGrupo, const double speed, const double cercania) {
    cec17_init("algPropio", funcid, dim);
    int maxEvals = 10000 * dim;
    int evals = 0;
    
    int numGrupos = tamPoblacion/tamGrupo;

    // Generar poblacion inicial
    vector<Solucion> poblacion = getPoblacionInicial(dim, tamPoblacion);

    // Evaluar poblacion inicial
    for (int i=0; i<tamPoblacion; i++) 
        poblacion[i].fitness = cec17_fitness(&poblacion[i].v[0]);


    // Aumentamos las evaluaciones de la solucion inicial
    evals += tamPoblacion;
    ofstream fichDiver;
    string filePath = "./results_algPropio/divergencia_"+to_string(funcid)+"_"+to_string(dim)+".txt";
    fichDiver.open (filePath, std::ios_base::app);
    fichDiver<<"Inicio"<<endl;
    while (evals < maxEvals) {
        // El vector guarda la posicion de las soluciones eliminadas en esta iteracion, para luego remplazarlos por nuevas.
        vector<int> eliminados;

        // Desordenamos la poblacion para tener grupos diferentes
        random_shuffle( poblacion.begin(), poblacion.end() );

        for (int i=0; i<numGrupos && (evals<maxEvals); i++) {

            // Encontrar mejor del grupo
            int best = 0;
            for (int j=1; j<tamGrupo; j++) {
                if (poblacion[i*tamGrupo + j].fitness < poblacion[i*tamGrupo + best].fitness) 
                    best = j;
            }

            for (int j=0; j<tamGrupo && (evals<maxEvals); j++) {
                if (j != best) {
                    int posBest = i*tamGrupo + best;
                    int posComparar = i*tamGrupo + j;
                    moveInDirectionOf(poblacion[posComparar], poblacion[posBest], speed);

                    // Actualizamos de la solucion movida.
                    poblacion[posComparar].fitness = cec17_fitness(&poblacion[posComparar].v[0]);
                    evals++;

                    // Si se acerca mas que la constante de cercania, se unen.
                    double dist = getDist(poblacion[posComparar].v, poblacion[posBest].v);

                    if (dist < cercania) {
                        
                        // Si la solucion que estamos comparando es mejor que el mejor que teniamos, actualizamos el best.
                        // Para asi al unir las soluciones quedarnos con la mejor.
                        if (poblacion[posComparar].fitness < poblacion[posBest].fitness) {
                            int aux;
                            aux = posBest;
                            posBest = posComparar;
                            posComparar = aux;

                            best = j;
                        }

                        // Aumentamos el peso de la mejor solucion y eliminamos el otro.
                        poblacion[posBest].peso += poblacion[posComparar].peso;
                        if (poblacion[posBest].peso > 20)
                            poblacion[posBest].peso = 20;
                        eliminados.push_back(posComparar);
                    }
                }
            }
        }

        // Para las soluciones eliminadas generamos nuevas soluciones.
        for (int i=0; i<eliminados.size() && (evals<maxEvals); i++) {
            poblacion[eliminados[i]] = getSolucionAleatoria(dim);
            poblacion[eliminados[i]].fitness = cec17_fitness(&poblacion[eliminados[i]].v[0]);
            evals++;
        }
        fichDiver<<"Divergencia\t"<<getDiversity(poblacion)<<endl;
    }
}




int main()
{
    string entryParameters = "./bin/parametros.txt";              // Fichero en donde estan guardados todos los parametros de entrada.

    // Leemos las semillas que se usaran para las 10 ejecuciones.
    vector<unsigned int> seeds = readVectorUnsigned(entryParameters, "SEEDS");

    // Leemos las dimensiones del problema que queremos resolver
    vector<unsigned int> dims = readVectorUnsigned(entryParameters, "DIMS");

    // Leemos el tamaño de la poblacion de soluciones a usar.
    int tamPoblacion = readInt(entryParameters, "TAMPOBLACION");

    // Leemos el tamaño de los grupos en los que dividimos la poblacion. Tiene que ser divisor de tamPoblacion.
    int tamGrupo = readInt(entryParameters, "TAMGRUPO");

    // Leemos la velocidad que queremos que tengan las soluciones.
    double speed = readDouble(entryParameters, "SPEED");

    // Leemos el constante de cercania para unir soluciones.
    double cercania = readDouble(entryParameters, "CERCANIA");

    // Para cada dimension
    for (int i=0; i<dims.size(); i++) {

        // Para cada uno de las 30 funciones
        for (int funcid = 1; funcid <= 30; funcid++) {

            // Se repite 10 veces con 10 semillas diferentes
            for (int j=0; j<10; j++) {
                srand(seeds[j]);

                cout<<"Dim: "<<dims[i]<<" Function ID: "<<funcid<<endl;
                
                // Inicializo el timer
                start_timers();

                // llamada al algoritmo
                algPropio( funcid, dims[i], tamPoblacion, tamGrupo, speed, cercania);

                // Mostrar el tiempo.
                cout << "Tiempo transcurrido: " << elapsed_time(TIMER_TYPE::VIRTUAL)<<"s" << endl;
            }
        }
    }
}