#ifndef SIMULATION_HPP
#define SIMULATION_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Simulation {
public:
	Simulation();
	Simulation(int N, float g, int seed);
	void update(float dt);
	void draw();
	~Simulation();
private:
	int N;
	float g;
	std::vector<float> xParticles, vParticles, mass;
	GLuint vertexBuffer, colorBuffer, massBuffer;
};

#endif