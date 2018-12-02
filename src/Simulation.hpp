#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "DistributionDisk.hpp"
#include "Star.hpp"

//Class for handling basic simulation of a galaxy
class Simulation {
public:
	//stars: pregenerated list of stars, NCloud: amount of clouds, g: gravitational constant,
	//hr: characteristic radius, hz: characteristic height
	Simulation(std::vector<Star>& stars, int NCloud, float g, float hr, float hz, int seed, GLuint programID);
	void update(float dt);
	void draw();
	void updateLuminosityBuffer(std::vector<Star>& stars);
	void reset(); //Regenerate to new initial state
	~Simulation();
private:
	int N, NCloud; //Amount of stars and clouds
	float g, totmass;
	std::vector<glm::vec4> xParticles, vParticles, colorBufferData; //Vectors for position, velocity, colour
	std::vector<float> mass; //Masses
	GLuint nID, mgID, dtID; //Uniforms in compute shader
	GLuint velocityBuffer, massBuffer, luminosityBuffer, vertexBuffer, colorBuffer, isCloudBuffer; //Buffers
	GLuint computeProgram, velocityTargetBuffer, vertexTargetBuffer; //More buffers
	GLuint cloudTextureID, cloudTextureSamplerID; //Texture
	DistributionDisk dist;
};

#endif