#ifndef TREE_HPP
#define TREE_HPP

#include "Branch.hpp"
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Tree {
public:
	Tree();
	Tree(float initialWidth, float maxDWidth, float dHeight, int divisions, int seed, int maxBranches, int tier);
	void draw(glm::mat4 mat, GLuint matID);
	~Tree();
private:
	/*std::vector<std::vector<Branch>> branches;
	std::vector<std::vector<float>> heights;
	std::vector<std::vector<float>> phis;
	std::vector<std::vector<float>> thetas;*/
	std::vector<Branch> branches;
	std::vector<glm::mat4> matrices;
};

#endif