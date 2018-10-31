#include "DistributionMass.hpp"

#include <cmath>

//Based on Salpeter initial mass function
DistributionMass::DistributionMass(int seed):
	mMin(1.0f), mMax(2.0f), dre(std::default_random_engine()), dist(std::uniform_real_distribution<float>(0, 1)),
	a(pow(mMin, -1.35f)), b(a - pow(20.0f, -1.35f)), c(-1.0f / 1.35f){
	dre.seed(seed);
}

void DistributionMass::setMs(float mMin, float mMax){
	this->mMin = mMin;
	this->mMax = mMax;
	a = pow(mMin, -1.35f);
	b = a - pow(20.0f, -1.35f);
}

float DistributionMass::evalMass(){
	float i = dist(dre);
	return pow(a - b * i, c);
}