#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"
#include "Star.hpp"

class Simulation {
public:
	Simulation(std::vector<Star>& stars, int NCloud, float g, float hr, float hz, int seed, GLuint programID);
	void update(float dt);
	void draw();
	void updateLuminosityBuffer(std::vector<Star>& stars);
	void reset();
	~Simulation();
private:
	int N, NCloud;
	float g, hr, hz, totmass;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData;
	std::vector<float> mass;
	GLuint nID, mgID, dtID;
	GLuint velocityBuffer, massBuffer, luminosityBuffer, vertexBuffer, colorBuffer, isCloudBuffer;
	GLuint computeProgram, velocityTargetBuffer, vertexTargetBuffer;
	GLuint cloudTextureID, cloudTextureSamplerID;
	DistributionDisk dist;
};

#endif