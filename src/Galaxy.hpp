#ifndef GALAXY_HPP
#define GALAXY_HPP

#include <vector>
#include <glad/glad.h>
#include "Colour.hpp"
#include "Simulation.hpp"
#include "Star.hpp"

class Galaxy{
public:
	Galaxy(int N, int NCloud, float g, float hr, float hz, int seed, GLuint programID);
	void update(float dt);
	void draw();
	void killAll();
	void reset();
private:
	int N, NCloud;
	std::vector<Star> stars;
	Simulation sim;
};

#endif