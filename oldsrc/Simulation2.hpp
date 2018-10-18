#ifndef Simulation2_HPP
#define Simulation2_HPP

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
	GLuint nID, gID, dtID;
	GLuint velocityBuffer, velocityTargetBuffer, massBuffer;
	GLuint vertexBuffer, vertexTargetBuffer, colorBuffer, computeProgram;
	DistributionDisk dist;
};

#endif