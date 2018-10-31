#ifndef DISTRIBUTIONDISK_HPP
#define DISTRIBUTIONDISK_HPP

#include <random>
#include <glm/glm.hpp>

class DistributionDisk {
public:
	DistributionDisk(int seed);
	void setH(float hr, float hz);
	glm::vec4 evalPos();
private:
	float hr, hz;
	std::default_random_engine dre;
	std::uniform_real_distribution<float> dist;
};

#endif