#ifndef SimulationSingle_HPP
#define SimulationSingle_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"

class SimulationSingle {
public:
	SimulationSingle();
	SimulationSingle(int N, float g, float hr, float hz, int seed);
	void update(float dt);
	void draw();
	~SimulationSingle();
private:
	int N;
	float g, hr, hz;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData;
	std::vector<float> mass;
	GLuint nID, gID, dtID;
	GLuint velocityBuffer, velocityTargetBuffer, massBuffer, massTargetBuffer, vertexBuffer, vertexTargetBuffer, colorBuffer;
	GLuint computeProgram;
	DistributionDisk dist;
};

#endif