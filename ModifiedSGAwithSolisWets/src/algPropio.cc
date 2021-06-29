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



void clip(vector<double> &sol, int lower, int upper){
    for (auto &val : sol){
        if (val < lower){
            val = lower;
        }
        else if (val > upper){
            val = upper;
        }
    }
}

void increm_bias(vector<double> &bias, vector<double> dif){
    for (unsigned i = 0; i < bias.size(); i++){
        bias[i] = 0.2 * bias[i] + 0.4 * (dif[i] + bias[i]);
    }
}

void decrement_bias(vector<double> &bias, vector<double> dif){
    for (unsigned i = 0; i < bias.size(); i++){
        bias[i] = bias[i] - 0.4 * (dif[i] + bias[i]);
    }
}

/**
 * Aplica el Solis Wets
 *
 * @param  sol solucion a mejorar.
 * @param fitness fitness de la solución.
 */
template <class Random>
void soliswets(vector<double> &sol, double &fitness, double delta, int maxevals, int lower, int upper, Random &random){
    const size_t dim = sol.size();
    vector<double> bias(dim), dif(dim), newsol(dim);
    double newfit;
    size_t i;

    int evals = 0;
    int num_success = 0;
    int num_failed = 0;

    while (evals < maxevals){
        std::uniform_real_distribution<double> distribution(0.0, delta);

        for (i = 0; i < dim; i++){
            dif[i] = distribution(random);
            newsol[i] = sol[i] + dif[i] + bias[i];
        }

        clip(newsol, lower, upper);
        newfit = cec17_fitness(&newsol[0]);
        evals += 1;

        if (newfit < fitness){
            sol = newsol;
            fitness = newfit;
            increm_bias(bias, dif);
            num_success += 1;
            num_failed = 0;
        }
        else if (evals < maxevals) {

            for (i = 0; i < dim; i++) {
                newsol[i] = sol[i] - dif[i] - bias[i];
            }

            clip(newsol, lower, upper);
            newfit = cec17_fitness(&newsol[0]);
            evals += 1;

            if (newfit < fitness) {
                sol = newsol;
                fitness = newfit;
                decrement_bias(bias, dif);
                num_success += 1;
                num_failed = 0;
            }
            else {
                for (i = 0; i < dim; i++) {
                    bias[i] /= 2;
                }

                num_success = 0;
                num_failed += 1;
            }
        }

        if (num_success >= 5) {
            num_success = 0;
            delta *= 2;
        }
        else if (num_failed >= 3) {
            num_failed = 0;
            delta /= 2;
        }
    }
}



void algPropio(const int funcid, const int dim, const int tamPoblacion, const int tamGrupo, const double speed, const double cercania, unsigned seed) {
    cec17_init("algPropio", funcid, dim);
    srand(seed);
    std::mt19937 gen(seed*2); // Inicio semilla para BL
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
    int uniones = 0;
    int bls = 0;
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

            // Encontrar primer y segundo mejor del grupo
            int best;
            int secondBest;
            if (poblacion[i*tamGrupo].fitness < poblacion[i*tamGrupo + 1].fitness) {
                best = 0;
                secondBest = 1;
            } else {
                best = 1;
                secondBest = 0;
            }
            for (int j=2; j<tamGrupo; j++) {
                if (poblacion[i*tamGrupo + j].fitness < poblacion[i*tamGrupo + best].fitness) {
                    secondBest = best;
                    best = j;
                } else if (poblacion[i*tamGrupo + j].fitness < poblacion[i*tamGrupo + secondBest].fitness)
                    secondBest = j;
            }
            // Con probablidad 0.1 vemos si nos movemos al segundo mejor en vez del mejor.
            int moveto = best;
            if (double(rand())/double(RAND_MAX) < 0.1)
                moveto = secondBest;
            for (int j=0; j<tamGrupo && (evals<maxEvals); j++) {
                if (j != moveto) {
                    int posMoveto = i*tamGrupo + best;
                    int posComparar = i*tamGrupo + j;
                    moveInDirectionOf(poblacion[posComparar], poblacion[posMoveto], speed);

                    // Actualizamos de la solucion movida.
                    poblacion[posComparar].fitness = cec17_fitness(&poblacion[posComparar].v[0]);
                    evals++;

                    // Si se acerca mas que la constante de cercania, se unen.
                    double dist = getDist(poblacion[posComparar].v, poblacion[posMoveto].v);

                    if (dist < cercania) {
                        
                        // Si la solucion que estamos comparando es mejor que el mejor que teniamos, actualizamos el moveto.
                        // Para asi al unir las soluciones quedarnos con la mejor.
                        if (poblacion[posComparar].fitness < poblacion[posMoveto].fitness) {
                            int aux;
                            aux = posMoveto;
                            posMoveto = posComparar;
                            posComparar = aux;

                            moveto = j;
                        }

                        // Si es la primera vez que el peso de la solucion va a ser mas de 20, aplicamos BL sobre ella.
                        if (poblacion[posMoveto].peso < 20 && poblacion[posComparar].peso+poblacion[posMoveto].peso>=20) {
                            soliswets(poblacion[posMoveto].v, poblacion[posMoveto].fitness, 0.2, min(200, maxEvals-evals), -100, 100, gen);
                            evals += min(200, maxEvals-evals);
                            bls++;
                        }
                        // Aumentamos el peso de la mejor solucion y eliminamos el otro.
                        poblacion[posMoveto].peso += poblacion[posComparar].peso;
                        if (poblacion[posMoveto].peso > 20) 
                            poblacion[posMoveto].peso = 20;
                        eliminados.push_back(posComparar);
                        uniones++;
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
    cout<<"Uniones: "<<uniones<<endl;
    cout<<"BLs: "<<bls<<endl;
    fichDiver.close();
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

                cout<<"Dim: "<<dims[i]<<" Function ID: "<<funcid<<endl;

                // Inicializo el timer
                start_timers();

                // llamada al algoritmo
                algPropio( funcid, dims[i], tamPoblacion, tamGrupo, speed, cercania, seeds[j]);

                // Mostrar el tiempo.
                cout << "Tiempo transcurrido: " << elapsed_time(TIMER_TYPE::VIRTUAL)<<"s" << endl;
            }
        }
    }
}