#include "Galaxy.hpp"
#include "DistributionMass.hpp"
#include <cmath>
#include <iostream>

const float mMin = 0.2f, mMax = 10.0f;

std::vector<Star> generateStars(int N, int seed, float g){
	std::vector<Star> stars = std::vector<Star>(N, Star(1, 1, g));
	DistributionMass mDist(seed);
	mDist.setMs(mMin, mMax);
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

void Galaxy::killAll(){
	for(int i = 0;i < (int) stars.size();i++) if(stars[i].getStage() == 2) stars[i].setAge(stars[i].getTC2() - 0.05);
}

void Galaxy::reset(){
	for(int i = 0;i < (int) stars.size();i++) stars[i].reset();
}