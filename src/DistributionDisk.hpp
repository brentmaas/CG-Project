#ifndef DISTRIBUTIONDISK_HPP
#define DISTRIBUTIONDISK_HPP

#include <random>
#include <glm/glm.hpp>

class DistributionDisk {
public:
	DistributionDisk();
	DistributionDisk(int seed);
	void setH(float hr, float hz);
	glm::vec4 evalPos();
private:
	std::default_random_engine dre;
	std::uniform_real_distribution<float> dist;
	float hr, hz;
};

#endif