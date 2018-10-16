#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"

class SimulationSimple {
public:
	SimulationSimple();
	Simulation(int N, float g, int seed);
	void update(float dt);
	void draw();
	~SimulationSimple();
private:
	int N;
	float g;
	std::vector<float> xParticles, vParticles, colorBufferData, mass;
	GLuint vertexBuffer, colorBuffer, massBuffer;
	DistributionDisk dist;
};

#endif