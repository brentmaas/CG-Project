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
	SimulationSimple(int N, int NCloud, float g, float hr, float hz, int seed);
	SimulationSimple(std::vector<Star>& stars, int NCloud, float g, float hr, float hz, int seed);
	void update(float dt);
	void draw();
	void updateRadiusBuffer(std::vector<Star>& stars);
	void updateLuminosityBuffer(std::vector<Star>& stars);
	void updateStageBuffer(std::vector<Star>& stars);
	~SimulationSimple();
private:
	int N, NCloud;
	float g, hr, hz, totmass;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData;
	std::vector<float> mass;
	GLuint nID, mID, gID, dtID;
	GLuint velocityBuffer, velocityTargetBuffer, massBuffer, radiusBuffer, luminosityBuffer, vertexBuffer, vertexTargetBuffer, colorBuffer, stageBuffer, isCloudBuffer, computeProgram;
	DistributionDisk dist;
};

#endif