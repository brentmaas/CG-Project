#ifndef BRANCH_HPP
#define BRANCH_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Branch {
public:
	Branch();
	Branch(float initialWidth, float maxDWidth, float dHeight, int divisions, int seed);
	void draw();
	~Branch();
	float height;
private:
	int vertices;
	GLuint vertexBuffer, colorBuffer;
	int numBranches;
	std::vector<Branch> branches;
	std::vector<float> branchHeights;
	std::vector<float> phis;
	std::vector<float> thetas;
};

#endif