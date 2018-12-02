#ifndef DISTRIBUTIONDISK_HPP
#define DISTRIBUTIONDISK_HPP

#include <random>
#include <glm/glm.hpp>

//Class for the spatial distribution of stars within a disc galaxy
//Source: en.wikipedia.org/wiki/Galactic_disc
class DistributionDisk {
public:
	DistributionDisk(int seed, int hr, int hz);
	//Generate a random position for a star as (x, y, z, 1)
	glm::vec4 evalPos();
private:
	//Characteristic radius/height
	float hr, hz;
	//Random distributions
	std::default_random_engine dre;
	std::uniform_real_distribution<float> dist;
};

#endif