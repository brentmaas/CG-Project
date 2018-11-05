#include "Galaxy.hpp"
#include "DistributionMass.hpp"
#include <cmath>
#include <iostream>

const float mMin = 0.5f, mMax = 10.0f;

std::vector<Star> generateStars(int N, int seed, float g){
	std::vector<Star> stars = std::vector<Star>(N, Star(1, 1, g));
	DistributionMass mDist(seed);
	mDist.setMs(mMin, mMax);
	//float r0 = 1.5f * pow(mMax / rhoC, 1.0f / 3.0f);
	for(int i = 0;i < N;i++){
		float m = mDist.evalMass();
		stars[i].setMR(m, 5 * pow(m, 1.0f / 3.0f));
		//stars[i].setMR(m, r0);
	}
	float minAge = stars[0].getTC();
	for(int i = 1;i < N;i++) if(stars[i].getTC() < minAge) minAge = stars[i].getTC();
	for(int i = 0;i < N;i++) stars[i].addAge(minAge - 0.1f);
	return stars;
}

Galaxy::Galaxy(int N, float g, float hr, float hz, int seed):
	N(N), stars(generateStars(N, seed, g)), sim(stars, g, hr, hz, seed){
	
}

void Galaxy::update(float dt){
	for(int i = 0;i < N;i++) stars[i].update(dt);
	sim.updateRadiusBuffer(stars);
	sim.updateLuminosityBuffer(stars);
	sim.updateStageBuffer(stars);
	sim.update(dt);
}

void Galaxy::draw(){
	sim.draw();
}