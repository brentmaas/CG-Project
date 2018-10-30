#include "Galaxy.hpp"

Galaxy::Galaxy(){
	N = 0;
}

Galaxy::Galaxy(int N, float g, float hr, float hz, int seed){
	this->N = N;
	stars = std::vector<Star>(N);
	for(int i = 0;i < N;i++){
		stars[i] = Star(5.0f, 1);
	}
	sim = SimulationSimple(stars, g, hr, hz, seed);
}