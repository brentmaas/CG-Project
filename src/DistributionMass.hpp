#ifndef DISTRIBUTIONMASS_HPP
#define DISTRIBUTIONMASS_HPP

#include <random>

//Class for the mass distribution (in solar masses) of stars within a galaxy
//Source: en.wikipedia.org/wiki/Initial_mass_function#Salpeter_(1955)
class DistributionMass {
public:
	DistributionMass(int seed, float mMin, float mMax);
	//Generate a random mass (in solar masses) for a star
	float evalMass();
private:
	//Minimal and maximal mass
	float mMin, mMax;
	//Random distribution
	std::default_random_engine dre;
	std::uniform_real_distribution<float> dist;
	//Constants in mass distribution
	float a, b, c;
};

#endif