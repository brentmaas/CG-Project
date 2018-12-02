#include "DistributionMass.hpp"

#include <cmath>

//Source: en.wikipedia.org/wiki/Initial_mass_function#Salpeter_(1955)
DistributionMass::DistributionMass(int seed, float mMin, float mMax):
	mMin(mMin), mMax(mMax), dre(std::default_random_engine()), dist(std::uniform_real_distribution<float>(0, 1)),
	a(pow(mMin, -1.35f)), b(a - pow(20.0f, -1.35f)), c(-1.0f / 1.35f){
	dre.seed(seed);
}

//Generate a random mass (in solar masses) for a star
float DistributionMass::evalMass(){
	float i = dist(dre);
	return pow(a - b * i, c);
}