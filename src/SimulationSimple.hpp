#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"
#include "Star.hpp"

class SimulationSimple {
public:
	SimulationSimple();
	SimulationSimple(int N, float g, float hr, float hz, int seed);
	SimulationSimple(std::vector<Star>& stars, float g, float hr, float hz, int seed);
	void update(float dt);
	void draw();
	~SimulationSimple();
private:
	int N;
	float g, hr, hz, totmass;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData;
	std::vector<float> mass;
	GLuint nID, mID, gID, dtID;
	GLuint velocityBuffer, velocityTargetBuffer, massBuffer, vertexBuffer, vertexTargetBuffer, colorBuffer, computeProgram;
	DistributionDisk dist;
};

#endif