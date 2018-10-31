#include "Galaxy.hpp"
#include "DistributionMass.hpp"
#include <cmath>

std::vector<Star> generateStars(int N, int seed){
	std::vector<Star> stars = std::vector<Star>(N, Star(1, 1));
	DistributionMass mDist(seed);
	mDist.setMs(0.5f, 20.0f);
	for(int i = 0;i < N;i++){
		float m = mDist.evalMass();
		stars[i].setMR(m, 5 * pow(m, 1.0f / 3.0f));
	}
	return stars;
}

Galaxy::Galaxy(int N, float g, float hr, float hz, int seed):
	N(N), stars(generateStars(N, seed)), sim(stars, g, hr, hz, seed){
	
}

void Galaxy::update(float dt){
	for(int i = 0;i < N;i++) stars[i].update(dt);
	sim.update(dt);
}

void Galaxy::draw(){
	sim.draw();
}