#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"

class Simulation2 {
public:
	Simulation2();
	Simulation2(int N, float g, float hr, float hz, int seed);
	void update(float dt);
	void draw();
	~Simulation2();
private:
	int N;
	float g, hr, hz;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData, dForce;
	std::vector<float> mass;
	GLuint nID, gID, n2ID, dt2ID;
	GLuint velocityBuffer, massBuffer;
	GLuint vertexBuffer, colorBuffer, dForceBuffer, computeProgram, compute2Program;
	DistributionDisk dist;
};

#endif