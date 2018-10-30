#ifndef GALAXY_HPP
#define GALAXY_HPP

#include <vector>
#include <GL/glew.h>
#include "SimulationSimple.hpp"
#include "Colour.hpp"
#include "Star.hpp"

class Galaxy{
public:
	Galaxy();
	Galaxy(int N, float g, float hr, float hz, int seed);
	SimulationSimple sim;
private:
	std::vector<Star> stars;
	int N;
};

#endif