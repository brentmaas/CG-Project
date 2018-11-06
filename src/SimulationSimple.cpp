#include "SimulationSimple.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

#include "Colour.hpp"
#include "Star.hpp"

const float PI  = 3.141592f;
GLuint loadComputeShader(const char* file, GLuint type){
	GLuint shaderID = glCreateShader(type);
	
	std::string shaderSrc;
	std::ifstream shaderStream(file, std::ios::in);
	if(shaderStream.is_open()){
		std::stringstream strStream;
		strStream << shaderStream.rdbuf();
		shaderSrc = strStream.str();
		shaderStream.close();
	}else{
		std::cerr << "Could not open" << file << std::endl;
		getchar();
		return 0;
	}
	
	char const * srcPtr = shaderSrc.c_str();
	glShaderSource(shaderID, 1, &srcPtr, NULL);
	glCompileShader(shaderID);
	
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0){
		std::vector<char> errMsg(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errMsg[0]);
		std::cerr << &errMsg[0] << std::endl;
	}
	
	return shaderID;
}

GLuint generateComputeProgram(const char* computeFile){
	GLuint computeShaderID = loadComputeShader(computeFile, GL_COMPUTE_SHADER);
	
	GLuint programID = glCreateProgram();
	glAttachShader(programID, computeShaderID);
	glLinkProgram(programID);
	
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0){
		std::vector<char> errMsg(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errMsg[0]);
		std::cerr << &errMsg[0] << std::endl;
	}
	
	glDetachShader(programID, computeShaderID);
	glDeleteShader(computeShaderID);
	
	return programID;
}

SimulationSimple::SimulationSimple(int N, float g, float hr, float hz, int seed):
	N(N), g(g), hr(hr), hz(hz), dist(seed){
	srand(seed);
	dist.setH(this->hr, this->hz);
	xParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	colorBufferData = std::vector<glm::vec4>(N, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mass = std::vector<float>(N, 5.0f);
	
	for(int i = 0;i < N;i++){
		glm::vec4 pos = dist.evalPos();
		glm::vec4& x = xParticles[i];
		x.x = pos.x;
		x.y = pos.z;
		x.z = pos.y;
		glm::vec4& c = colorBufferData[i];
		float T = 1000 + 10000 * (float) rand() / RAND_MAX;
		c = getColour(T);
	}
	
	glm::vec3 mmp = glm::vec3(0, 0, 0);
	totmass = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x = xParticles[i];
		float m = mass[i];
		mmp += glm::vec3(m * x.x, m * x.y, m * x.z);
		totmass += m;
	}
	mmp /= totmass;
	for(int i = 0;i < N;i++){
		glm::vec4& x = xParticles[i];
		x.x -= mmp.x;
		x.y -= mmp.y;
		x.z -= mmp.z;
		float r = sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
		glm::vec4& v = vParticles[i];
		if(r <= 0){
			v = glm::vec4(0, 0, 0, 0);
			continue;
		}
		float vtot = sqrt(g * totmass / r);
		float rproj = sqrt(x.x * x.x + x.z * x.z);
		float costheta = glm::dot(glm::vec3(x.x / r, x.y / r, x.z / r), glm::vec3(x.x / rproj, 0, x.z / rproj));
		float vproj = vtot * costheta;
		v.x = vproj * x.z / rproj;
		v.y = ((x.y - mmp.y < 0) - (x.y - mmp.y > 0)) * vtot * sqrt(std::max(0.0f, 1 - costheta * costheta));
		v.z = -vproj * x.x / rproj;
	}
	
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), xParticles.data());
	
	glGenBuffers(1, &vertexTargetBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexTargetBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), xParticles.data());
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colorBufferData.size() * sizeof(glm::vec4), colorBufferData.data());
	
	std::vector<glm::ivec4> stageBufferData(N, glm::ivec4(0));
	glGenBuffers(1, &stageBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, stageBuffer);
	glBufferData(GL_ARRAY_BUFFER, stageBufferData.size() * sizeof(glm::ivec4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, stageBufferData.size() * sizeof(glm::ivec4), stageBufferData.data());
	
	computeProgram = generateComputeProgram("resources/shaders/particles_simple.compute");
	mID = glGetUniformLocation(computeProgram, "m");
	gID = glGetUniformLocation(computeProgram, "g");
	dtID = glGetUniformLocation(computeProgram, "dt");
	nID = glGetUniformLocation(computeProgram, "N");
	
	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), vParticles.data());
	
	glGenBuffers(1, &velocityTargetBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityTargetBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), vParticles.data());
	
	glGenBuffers(1, &massBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, massBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
	
	std::vector<float> radii(N);
	for(int i = 0;i < N;i++) radii[i] = 5 * pow(mass[i], 1.0f / 3.0f);
	glGenBuffers(1, &radiusBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, radiusBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, radii.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, radii.size() * sizeof(GLfloat), radii.data());
	
	std::vector<float> lums(N);
	for(int i = 0;i < N;i++) lums[i] = 1;
	glGenBuffers(1, &luminosityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, luminosityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lums.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, lums.size() * sizeof(GLfloat), lums.data());
}

SimulationSimple::SimulationSimple(std::vector<Star>& stars, float g, float hr, float hz, int seed):
	N(stars.size()), g(g), hr(hr), hz(hz), dist(seed){
	srand(seed);
	dist.setH(this->hr, this->hz);
	xParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	colorBufferData = std::vector<glm::vec4>(N, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mass = std::vector<float>(N, 1.0f);
	for(int i = 0;i < N;i++){
		colorBufferData[i] = getColour(stars[i].T());
		mass[i] = stars[i].getM();
	}
	
	for(int i = 0;i < N;i++){
		glm::vec4 pos = dist.evalPos();
		glm::vec4& x = xParticles[i];
		x.x = pos.x;
		x.y = pos.z;
		x.z = pos.y;
		glm::vec4& c = colorBufferData[i];
		float T = 1000 + 10000 * (float) rand() / RAND_MAX;
		c = getColour(T);
	}
	
	glm::vec3 mmp = glm::vec3(0, 0, 0);
	totmass = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x = xParticles[i];
		float m = mass[i];
		mmp += glm::vec3(m * x.x, m * x.y, m * x.z);
		totmass += m;
	}
	mmp /= totmass;
	for(int i = 0;i < N;i++){
		glm::vec4& x = xParticles[i];
		x.x -= mmp.x;
		x.y -= mmp.y;
		x.z -= mmp.z;
		float r = sqrt(x.x * x.x + x.y * x.y + x.z * x.z);
		glm::vec4& v = vParticles[i];
		if(r <= 0){
			v = glm::vec4(0, 0, 0, 0);
			continue;
		}
		float vtot = sqrt(g * totmass / r);
		float rproj = sqrt(x.x * x.x + x.z * x.z);
		float costheta = glm::dot(glm::vec3(x.x / r, x.y / r, x.z / r), glm::vec3(x.x / rproj, 0, x.z / rproj));
		float vproj = vtot * costheta;
		v.x = vproj * x.z / rproj;
		v.y = ((x.y - mmp.y < 0) - (x.y - mmp.y > 0)) * vtot * sqrt(std::max(0.0f, 1 - costheta * costheta));
		v.z = -vproj * x.x / rproj;
	}
	
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), xParticles.data());
	
	glGenBuffers(1, &vertexTargetBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexTargetBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), xParticles.data());
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colorBufferData.size() * sizeof(glm::vec4), colorBufferData.data());
	
	std::vector<glm::ivec4> stageBufferData(N, glm::ivec4(0));
	//std::cout << stageBufferData[0].x << " " << stageBufferData[0].y << " " << stageBufferData[0].z << " " << stageBufferData[0].w << std::endl;
	glGenBuffers(1, &stageBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, stageBuffer);
	glBufferData(GL_ARRAY_BUFFER, stageBufferData.size() * sizeof(glm::ivec4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, stageBufferData.size() * sizeof(glm::ivec4), stageBufferData.data());
	
	computeProgram = generateComputeProgram("resources/shaders/particles_simple.compute");
	mID = glGetUniformLocation(computeProgram, "m");
	gID = glGetUniformLocation(computeProgram, "g");
	dtID = glGetUniformLocation(computeProgram, "dt");
	nID = glGetUniformLocation(computeProgram, "N");
	
	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), vParticles.data());
	
	glGenBuffers(1, &velocityTargetBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityTargetBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), vParticles.data());
	
	glGenBuffers(1, &massBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, massBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
	
	std::vector<float> radii = std::vector<float>(N);
	for(int i = 0;i < N;i++) radii[i] = stars[i].getR();
	glGenBuffers(1, &radiusBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, radiusBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, radii.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, radii.size() * sizeof(GLfloat), radii.data());
	
	std::vector<float> lums(N);
	for(int i = 0;i < N;i++) lums[i] = 1;
	glGenBuffers(1, &luminosityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, luminosityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lums.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, lums.size() * sizeof(GLfloat), lums.data());
}

void SimulationSimple::update(float dt){
	glUseProgram(computeProgram);
	glUniform1i(nID, N);
	glUniform1f(mID, totmass);
	glUniform1f(gID, g);
	glUniform1f(dtID, dt);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexTargetBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velocityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, velocityTargetBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	std::swap(vertexBuffer, vertexTargetBuffer);
	std::swap(velocityBuffer, velocityTargetBuffer);
}

void SimulationSimple::draw(){
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(2);
	//glBindBuffer(GL_ARRAY_BUFFER, radiusBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, luminosityBuffer);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, stageBuffer);
	glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, 0, (void*) 0);
	glDrawArrays(GL_POINTS, 0, N);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

void SimulationSimple::updateRadiusBuffer(std::vector<Star>& stars){
	std::vector<float> radii = std::vector<float>(N);
	for(int i = 0;i < N;i++) radii[i] = stars[i].getR();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, radiusBuffer);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, radii.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, radii.size() * sizeof(GLfloat), radii.data());
}

void SimulationSimple::updateLuminosityBuffer(std::vector<Star>& stars){
	std::vector<float> lums = std::vector<float>(N);
	for(int i = 0;i < N;i++) lums[i] = stars[i].L();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, luminosityBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, lums.size() * sizeof(GLfloat), lums.data());
}

void SimulationSimple::updateStageBuffer(std::vector<Star>& stars){
	std::vector<glm::ivec4> stageBufferData(N, glm::ivec4(0));
	for(int i = 0;i < N;i++) stageBufferData[i] = glm::ivec4(stars[i].getStage());
	glBindBuffer(GL_ARRAY_BUFFER, stageBuffer);
	//glBufferData(GL_ARRAY_BUFFER, stageBufferData.size() * sizeof(glm::ivec4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, stageBufferData.size() * sizeof(glm::ivec4), stageBufferData.data());
}

SimulationSimple::~SimulationSimple(){
	glDeleteBuffers(1, &velocityBuffer);
	glDeleteBuffers(1, &velocityTargetBuffer);
	glDeleteBuffers(1, &massBuffer);
	glDeleteBuffers(1, &radiusBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &vertexTargetBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteBuffers(1, &stageBuffer);
	glDeleteProgram(computeProgram);
}