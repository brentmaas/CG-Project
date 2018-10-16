#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"

class Simulation {
public:
	Simulation();
	Simulation(int N, float g, float hr, float hz, int seed);
	void update(float dt);
	void draw();
	~Simulation();
private:
	int N;
	float g, hr, hz;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData;
	std::vector<float> mass;
	GLuint mID, gID, mmpID, dtID;
	GLuint velocityBuffer, massBuffer;
	GLuint vertexBuffer, colorBuffer, computeProgram;
	DistributionDisk dist;
};

#endif