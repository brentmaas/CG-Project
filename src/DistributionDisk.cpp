#include "DistributionDisk.hpp"
#include <ctime>
#include <cmath>
#include <glm/glm.hpp>

const float PI = 3.141592f;

DistributionDisk::DistributionDisk(int seed, int hr, int hz):
	hr(hr), hz(hz), dre(std::default_random_engine()), dist(std::uniform_real_distribution<float>(0, 1)){
	dre.seed(seed);
}

//Generate a random position for a star as (x, y, z, 1)
//Source: en.wikipedia.org/wiki/Galactic_disc
glm::vec4 DistributionDisk::evalPos(){
	glm::vec4 res = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	//Generate [0,1] representation of r and z, and direction in the plane
	float dr = dist(dre);
	float dz = dist(dre);
	float phi = 2 * PI * dist(dre);
	//Calculate x, y, z from dr, dz and phi
	float r = -hr * log(1 - dr);
	res.x = r * cos(phi);
	res.y = r * sin(phi);
	if(dz <= 0.5f) res.z = -hz * log(1 - 2 * dz);
	else res.z = hz * log(2 * dz - 1);
	return res;
}