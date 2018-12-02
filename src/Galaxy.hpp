#ifndef GALAXY_HPP
#define GALAXY_HPP

#include <vector>
#include <glad/glad.h>
#include "Colour.hpp"
#include "Simulation.hpp"
#include "Star.hpp"

//Class for handling a set of stars
class Galaxy{
public:
	//N: amount of stars, NCloud: amount of clouds, g: gravitational constant,
	//hr: characteristic radius, hz: characteristic height
	Galaxy(int N, int NCloud, float g, float hr, float hz, int seed, GLuint programID);
	void update(float dt);
	void draw();
	//Function to trigger all available novas
	void killAll();
	//Function to regenerate the galaxy to an initial state
	void reset();
private:
	//N: amount of stars, NCloud: amount of clouds
	int N, NCloud;
	std::vector<Star> stars;
	Simulation sim;
};

#endif