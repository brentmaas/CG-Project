#include "Simulation.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <algorithm>

#include "Colour.hpp"
#include "Star.hpp"
#include "OpenSimplexNoise.hpp"

const float PI  = 3.141592f, E = 2.71828182846f;

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

Simulation::Simulation(std::vector<Star>& stars, int NCloud, float g, float hr, float hz, int seed, GLuint programID):
	N(stars.size()), NCloud(NCloud), g(g), hr(hr), hz(hz), dist(seed){
	srand(seed);
	dist.setH(this->hr, this->hz);
	xParticles = std::vector<glm::vec4>(N + NCloud, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	vParticles = std::vector<glm::vec4>(N + NCloud, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
	colorBufferData = std::vector<glm::vec4>(N + NCloud, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mass = std::vector<float>(N + NCloud, 1.0f);
	for(int i = 0;i < N;i++){
		colorBufferData[i] = getColour(stars[i].T());
		mass[i] = stars[i].getM();
	}
	
	for(int i = 0;i < N + NCloud;i++){
		glm::vec4 pos = dist.evalPos();
		glm::vec4& x = xParticles[i];
		x.x = pos.x;
		x.y = pos.z;
		x.z = pos.y;
		//Take random temperature for colours
		float T = 1000 + 10000 * (float) rand() / RAND_MAX;
		colorBufferData[i] = getColour(T);
	}
	
	glm::vec3 mmp = glm::vec3(0, 0, 0);
	totmass = 0;
	for(int i = 0;i < N + NCloud;i++){
		glm::vec4& x = xParticles[i];
		float m = mass[i];
		mmp += glm::vec3(m * x.x, m * x.y, m * x.z);
		totmass += m;
	}
	mmp /= totmass;
	for(int i = 0;i < N + NCloud;i++){
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
	
	computeProgram = generateComputeProgram("resources/shaders/particles_simple.compute");
	mgID = glGetUniformLocation(computeProgram, "mg");
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
	
	std::vector<float> lums(N + NCloud, 100000);
	glGenBuffers(1, &luminosityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, luminosityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lums.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, lums.size() * sizeof(GLfloat), lums.data());
	
	std::vector<glm::ivec4> isCloud(N + NCloud, glm::ivec4(0));
	for(int i = N;i < N + NCloud;i++) isCloud[i] = glm::ivec4(1);
	glGenBuffers(1, &isCloudBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, isCloudBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, isCloud.size() * sizeof(glm::ivec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, isCloud.size() * sizeof(glm::ivec4), isCloud.data());
	
	const int side = 1024;
	const float scaling = 0.1f, alphaScaling = 0.1f;
	std::vector<glm::vec4> data(side * side, glm::vec4(0.0f));
	OpenSimplexNoise noise(seed);
	//Gauss 4 sigma
	const float corr = pow(E, -4);
	for(int x = 0;x < side;x++){
		for(int y = 0;y < side;y++){
			float r2 = (x - side / 2) * (x - side / 2) + (y - side / 2) * (y - side / 2);
			if(r2 <= side * side / 4){
				float val = alphaScaling * 0.5 * (1 + noise.Evaluate((x - side / 2) * scaling, (y - side / 2)) * scaling);
				data[x + y * side].x = 1;
				data[x + y * side].y = 1;
				data[x + y * side].z = 1;
				data[x + y * side].w = (pow(E, -16 * r2 / (side * side)) - corr) * val;
			}
		}
	}
	
	glGenTextures(1, &cloudTextureID);
	glBindTexture(GL_TEXTURE_2D, cloudTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, side, side, 0, GL_RGBA, GL_FLOAT, data.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	cloudTextureSamplerID = glGetUniformLocation(programID, "texSampler");
}

void Simulation::update(float dt){
	glUseProgram(computeProgram);
	glUniform1i(nID, N + NCloud);
	glUniform1f(mgID, totmass * g);
	glUniform1f(dtID, dt);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vertexTargetBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, velocityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, velocityTargetBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N + NCloud, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	std::swap(vertexBuffer, vertexTargetBuffer);
	std::swap(velocityBuffer, velocityTargetBuffer);
}

void Simulation::draw(){
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, luminosityBuffer);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, isCloudBuffer);
	glVertexAttribPointer(3, 4, GL_INT, GL_FALSE, 0, (void*) 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cloudTextureID);
	glDrawArrays(GL_POINTS, 0, N + NCloud);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

void Simulation::updateLuminosityBuffer(std::vector<Star>& stars){
	std::vector<float> lums = std::vector<float>(N + NCloud, 100000);
	for(int i = 0;i < N;i++) lums[i] = stars[i].L();
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, luminosityBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, lums.size() * sizeof(GLfloat), lums.data());
}

Simulation::~Simulation(){
	glDeleteBuffers(1, &velocityBuffer);
	glDeleteBuffers(1, &velocityTargetBuffer);
	glDeleteBuffers(1, &massBuffer);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &vertexTargetBuffer);
	glDeleteBuffers(1, &colorBuffer);
	glDeleteProgram(computeProgram);
}