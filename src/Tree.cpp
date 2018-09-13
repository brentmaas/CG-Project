#include "Tree.hpp"
#include <cmath>

const float PI = 3.141592f;

Tree::Tree(){
	
}

Tree::Tree(float initialWidth, float maxDWidth, float dHeight, int divisions, int seed, int maxBranches, int tier){
	srand(seed);
	/*branches = std::vector<std::vector<Branch>>(tier + 1);
	
	branches[0] = {Branch(initialWidth, maxDWidth, dHeight, divisions, seed)};
	heights[0] = {0};
	phis[0] = {0};
	thetas[0] = {0};
	for(int i = 1;i < tier + 1;i++) for(int j = 0;j < (int) branches[i - 1].size();j++){
		int nBranches = rand() % (maxBranches + 1);
		branches[i] = std::vector<Branch>(nBranches);
		heights[i] = std::vector<float>(nBranches);
		phis[i] = std::vector<float>(nBranches);
		thetas[i] = std::vector<float>(nBranches);
		for(int k = 0;k < nBranches;k++){
			branches[i][k] = Branch(initialWidth / pow(2, i), maxDWidth, dHeight, divisions, seed);
			heights[i][k] = (0.5f + 0.5f * ((float) rand()) / RAND_MAX) * branches[i][k].height;
			phis[i][k] = ((float) rand()) / RAND_MAX * 2 * PI;
			thetas[i][k] = ((float) rand()) / RAND_MAX * PI / 2;
		}
	}*/
	branches = std::vector<Branch>();
	matrices = std::vector<glm::mat4>();
	branches.push_back(Branch(initialWidth, maxDWidth, dHeight, divisions, seed));
	matrices.push_back(glm::mat4(1.0f));
	std::vector<Branch> newBranches;
	for(int i = 1;i < tier + 1;i++) {
		int it = newBranches.size();
		for(int j = 0;j < it;j++){
			int nBranches = rand() % (maxBranches + 1);
			newBranches = std::vector<Branch>(nBranches);
		}
	}
}

void Tree::draw(glm::mat4 mat, GLuint matID){
	
}

Tree::~Tree(){
	
}