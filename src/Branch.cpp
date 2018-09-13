#include "Branch.hpp"
#include <cmath>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float PI = 3.141592f;

Branch::Branch(){
	vertexBuffer = 0;
	colorBuffer = 0;
	vertices = 0;
	numBranches = 0;
	height = 0;
}

Branch::Branch(float initialWidth, float maxDWidth, float dHeight, int divisions, int seed){
	srand(seed);
	std::vector<float> widths = {initialWidth};
	while(widths[widths.size() - 1] > 0){
		float a = widths[widths.size() - 1] - ((float) rand()) / RAND_MAX * maxDWidth;
		if(a < 0) a = 0;
		widths.push_back(a);
	}
	height = widths.size() * dHeight;
	
	vertices = 6 * divisions * (widths.size() - 1);
	std::vector<float> vertexBufferData(18 * divisions * (widths.size() - 1));
	for(int i = 0;i < (int) (widths.size() - 1);i++) for(int j = 0;j < (int) divisions;j++){
		vertexBufferData[18 * (divisions * i + j)] = widths[i] * cos(2 * PI * j / divisions);
		vertexBufferData[18 * (divisions * i + j) + 2] = widths[i] * sin(2 * PI * j / divisions);
		vertexBufferData[18 * (divisions * i + j) + 1] = dHeight * i;
		vertexBufferData[18 * (divisions * i + j) + 3] = widths[i] * cos(2 * PI * (j + 1) / divisions);
		vertexBufferData[18 * (divisions * i + j) + 5] = widths[i] * sin(2 * PI * (j + 1) / divisions);
		vertexBufferData[18 * (divisions * i + j) + 4] = dHeight * i;
		vertexBufferData[18 * (divisions * i + j) + 6] = widths[i + 1] * cos(2 * PI * j / divisions);
		vertexBufferData[18 * (divisions * i + j) + 8] = widths[i + 1] * sin(2 * PI * j / divisions);
		vertexBufferData[18 * (divisions * i + j) + 7] = dHeight * (i + 1);
		vertexBufferData[18 * (divisions * i + j) + 9] = widths[i] * cos(2 * PI * (j + 1) / divisions);
		vertexBufferData[18 * (divisions * i + j) + 11] = widths[i] * sin(2 * PI * (j + 1) / divisions);
		vertexBufferData[18 * (divisions * i + j) + 10] = dHeight * i;
		vertexBufferData[18 * (divisions * i + j) + 12] = widths[i + 1] * cos(2 * PI * (j + 1) / divisions);
		vertexBufferData[18 * (divisions * i + j) + 14] = widths[i + 1] * sin(2 * PI * (j + 1) / divisions);
		vertexBufferData[18 * (divisions * i + j) + 13] = dHeight * (i + 1);
		vertexBufferData[18 * (divisions * i + j) + 15] = widths[i + 1] * cos(2 * PI * j / divisions);
		vertexBufferData[18 * (divisions * i + j) + 17] = widths[i + 1] * sin(2 * PI * j / divisions);
		vertexBufferData[18 * (divisions * i + j) + 16] = dHeight * (i + 1);
	}
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(GLfloat), vertexBufferData.data(), GL_STATIC_DRAW);
	
	std::vector<float> colorBufferData(24 * divisions * widths.size(), 1.0f);
	for(int i = 0;i < (int) (6 * divisions * widths.size());i++){
		colorBufferData[4 * i] = 0.5f;
		colorBufferData[4 * i + 1] = 0.25f;
		colorBufferData[4 * i + 2] = 0.0f;
	}
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(GLfloat), colorBufferData.data(), GL_STATIC_DRAW);
	
	numBranches = 0;
}

void Branch::draw(){
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glDrawArrays(GL_TRIANGLES, 0, vertices);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

Branch::~Branch(){
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
}