#include "DistributionDisk.hpp"
#include <ctime>
#include <cmath>

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

float* DistributionDisk::eval(){
	float* res = new float[3];
	float dr = dist(dre);
	float dz = dist(dre);
	float phi = 2 * PI * dist(dre);
	float r = -hr * log(1 - dr);
	res[0] = r * cos(phi);
	res[1] = r * sin(phi);
	if(dz <= 0.5f) res[2] = -hz * log(1 - 2 * dz);
	else res[2] = hz * log(2 * dz - 1);
	return res;
}