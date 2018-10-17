#include "DistributionDisk.hpp"
#include <ctime>
#include <cmath>
#include <glm/glm.hpp>

const float PI = 3.141592f;

DistributionDisk::DistributionDisk(){
	dre = std::default_random_engine();
	dre.seed(time(0));
	dist = std::uniform_real_distribution<float>(0, 1);
	hr = 1;
	hz = 1;
}

DistributionDisk::DistributionDisk(int seed){
	dre = std::default_random_engine();
	dre.seed(seed);
	dist = std::uniform_real_distribution<float>(0, 1);
	hr = 1;
	hz = 1;
}

void DistributionDisk::setH(float hr, float hz){
	this->hr = hr;
	this->hz = hz;
}

glm::vec4 DistributionDisk::evalPos(){
	glm::vec4 res = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float dr = dist(dre);
	float dz = dist(dre);
	float phi = 2 * PI * dist(dre);
	//float r = -hr * log(1 - dr);
	float r = -hr * log((1 - dr) * 0.5f);
	res.x = r * cos(phi);
	res.y = r * sin(phi);
	if(dz <= 0.5f) res.z = -hz * log(1 - 2 * dz);
	else res.z = hz * log(2 * dz - 1);
	return res;
}