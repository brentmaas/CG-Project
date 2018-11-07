#ifndef GALAXY_HPP
#define GALAXY_HPP

#include <vector>
#include <GL/glew.h>
#include "SimulationSimple.hpp"
#include "Colour.hpp"
#include "Star.hpp"

class Galaxy{
public:
	Galaxy(int N, int NCloud, float g, float hr, float hz, int seed);
	void update(float dt);
	void draw();
private:
	int N, NCloud;
	std::vector<Star> stars;
	SimulationSimple sim;
};

#endif