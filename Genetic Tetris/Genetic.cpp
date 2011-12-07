#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()
#include <windows.h>
#include "Main.h"

#define GA_POPSIZE		32			// ga population size
#define GA_MAXITER		16384		// maximum iterations
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define GA_WEIGHTS		8

using namespace std;				// polluting global namespace, but hey...

struct ga_struct 
{
	vector<double> weights;			// the weights
	unsigned int fitness;			// its fitness
};

typedef vector<ga_struct> ga_vector;// for brevity

void init_population(ga_vector &population,
					 ga_vector &buffer ) 
{
	int tsize = GA_WEIGHTS;

	for (int i=0; i<GA_POPSIZE; i++) {
		ga_struct citizen;
		
		citizen.fitness = 0;

		for (int j=0; j<tsize; j++)
			citizen.weights.push_back(1.0);

		population.push_back(citizen);
	}

	buffer.resize(GA_POPSIZE);
}

void calc_fitness(ga_vector &population)
{
	unsigned int fitness;

	for (int i=0; i<GA_POPSIZE; i++) {
		population[i].fitness = runTetris(population[i].weights);
	}
}

bool fitness_sort(ga_struct x, ga_struct y) 
{ return (x.fitness < y.fitness); }

inline void sort_by_fitness(ga_vector &population)
{ sort(population.begin(), population.end(), fitness_sort); }

void elitism(ga_vector &population, 
				ga_vector &buffer, int esize )
{
	for (int i=0; i<esize; i++) {
		buffer[i].weights = population[i].weights;
		buffer[i].fitness = population[i].fitness;
	}
}

void mutate(ga_struct &member)
{
	int tsize = GA_WEIGHTS;
	int ipos = rand() % tsize;
	int delta = (rand() % 10)*0.5; 

	member.weights[ipos] += delta;
}

void mate(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int spos, i1, i2;

	elitism(population, buffer, esize);

	// Mate the rest
	for (int i=esize; i<GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % GA_WEIGHTS;

		for (int j = 0; j < spos; j++) {
			buffer[i].weights[j] = population[i1].weights[j];
		}

		for (int j = spos; j < GA_WEIGHTS; j++) {
			buffer[i].weights[j] = population[i1].weights[j];
		}

		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

inline void print_best(ga_vector &gav) { 
	cout << "Best: [";
	for (int i = 0; i < GA_WEIGHTS; i++) {
		cout << gav[0].weights[i] << ",";
	}
	
	cout << "] (" << gav[0].fitness << ")" << endl;
}

inline void swap(ga_vector *&population,
				 ga_vector *&buffer)
{ ga_vector *temp = population; population = buffer; buffer = temp; }

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(unsigned(time(NULL)));

	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;

	init_population(pop_alpha, pop_beta);
	population = &pop_alpha;
	buffer = &pop_beta;

	for (int i=0; i<GA_MAXITER; i++) {
		calc_fitness(*population);		// calculate fitness
		sort_by_fitness(*population);	// sort them
		print_best(*population);		// print the best one

		if ((*population)[0].fitness == 0) break;

		mate(*population, *buffer);		// mate the population together
		swap(population, buffer);		// swap buffers
	}

	return 0;
}