#include "Galaxy.hpp"
#include "DistributionMass.hpp"
#include <cmath>
#include <iostream>

const float mMin = 0.2f, mMax = 10.0f;

//Function to generate an amount of stars to use in the constructor's
//initialiser list
std::vector<Star> generateStars(int N, int seed, float g){
	std::vector<Star> stars = std::vector<Star>(N, Star(1, g));
	DistributionMass mDist(seed, mMin, mMax);
	for(int i = 0;i < N;i++){
		float m = mDist.evalMass();
		float r = pow(mDist.evalMass(), 1.0f / 3.0f);
		stars[i].setMR(m, r);
	}
	float minAge = stars[0].getTC();
	for(int i = 1;i < N;i++) if(stars[i].getTC() < minAge) minAge = stars[i].getTC();
	for(int i = 0;i < N;i++) stars[i].addAge(minAge - 0.05f);
	return stars;
}

//N: amount of stars, NCloud: amount of clouds, g: gravitational constant,
//hr: characteristic radius, hz: characteristic height
Galaxy::Galaxy(int N, int NCloud, float g, float hr, float hz, int seed, GLuint programID):
	N(N), NCloud(NCloud), stars(generateStars(N, seed, g)), sim(stars, NCloud, g, hr, hz, seed, programID){
	
}

void Galaxy::update(float dt){
	for(int i = 0;i < N;i++) stars[i].update(dt);
	sim.updateLuminosityBuffer(stars);
	sim.update(dt);
}

void Galaxy::draw(){
	sim.draw();
}

//Function to trigger all available novas
void Galaxy::killAll(){
	for(int i = 0;i < (int) stars.size();i++) if(stars[i].getStage() == 2) stars[i].setAge(stars[i].getTC2() - 0.05);
}

//Function to regenerate the galaxy to an initial state
void Galaxy::reset(){
	for(int i = 0;i < (int) stars.size();i++) stars[i].reset();
	sim.reset();
	sim.updateLuminosityBuffer(stars);
}