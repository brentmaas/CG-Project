#ifndef DISTRIBUTIONMASS_HPP
#define DISTRIBUTIONMASS_HPP

#include <random>

class DistributionMass {
public:
	DistributionMass(int seed);
	void setMs(float mMin, float mMax);
	float evalMass();
private:
	float mMin, mMax;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> dist;
	float a, b, c;
};

#endif