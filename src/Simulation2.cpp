#include "Simulation2.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

const float PI  = 3.141592f;
GLuint loadShader2(const char* file, GLuint type){
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

GLuint generateProgram(const char* computeFile){
	GLuint computeShaderID = loadShader2(computeFile, GL_COMPUTE_SHADER);
	
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

Simulation2::Simulation2(){
	N = 0;
	g = 1;
	nID = 0;
	gID = 0;
	n2ID = 0;
	dt2ID = 0;
	vertexBuffer = 0;
	colorBuffer = 0;
	massBuffer = 0;
	velocityBuffer = 0;
	dForceBuffer = 0;
	computeProgram = 0;
	compute2Program = 0;
}

Simulation2::Simulation2(int N, float g, int seed){
	srand(seed);
	dist = DistributionDisk(seed);
	dist.setH(5, 1);
	this->N = N;
	this->g = g;
	xParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	dForce = std::vector<glm::vec4>(N * N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	colorBufferData = std::vector<glm::vec4>(N, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	mass = std::vector<float>(N, 5.0f);
	for(int i = 0;i < N;i++){
		glm::vec4 pos = dist.eval();
		//xParticles[3 * i] = pos[0];
		//xParticles[3 * i + 1] = pos[2];
		//xParticles[3 * i + 2] = pos[1];
		glm::vec4& x = xParticles[i];
		x.x = pos.x;
		x.y = pos.z;
		x.z = pos.y;
		//vParticles[3 * i] = -pos[1] * sqrt(g * 5 * N);// * (pos[0] * pos[0] + pos[1] * pos[1]);
		//vParticles[3 * i + 2] = pos[0] * sqrt(g * 5 * N);// * (pos[0] * pos[0] + pos[1] * pos[1]);
		glm::vec4& v = vParticles[i];
		v.x = -pos.y * sqrt(N);
		v.z = pos.x * sqrt(N);
		//colorBufferData[4 * i] = ((float) rand()) / RAND_MAX;
		//colorBufferData[4 * i + 1] = ((float) rand()) / RAND_MAX;
		//colorBufferData[4 * i + 2] = ((float) rand()) / RAND_MAX;
		glm::vec4& c = colorBufferData[i];
		c.x = ((float) rand()) / RAND_MAX;
		c.y = ((float) rand()) / RAND_MAX;
		c.z = ((float) rand()) / RAND_MAX;
	}
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), xParticles.data());
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colorBufferData.size() * sizeof(glm::vec4), colorBufferData.data());
	
	computeProgram = generateProgram("resources/shaders/particles.compute");
	
	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), vParticles.data());
	
	glGenBuffers(1, &massBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, massBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
	
	compute2Program = generateProgram("resources/shaders/particles.compute2");
	
	glGenBuffers(1, &dForceBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dForceBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dForce.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dForce.size() * sizeof(glm::vec4), dForce.data());
	
	nID = glGetUniformLocation(computeProgram, "N");
	gID = glGetUniformLocation(computeProgram, "g");
	n2ID = glGetUniformLocation(compute2Program, "N");
	dt2ID = glGetUniformLocation(compute2Program, "dt");
}

void Simulation2::update(float dt){
	glUseProgram(computeProgram);
	glUniform1i(nID, N);
	glUniform1f(gID, g);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, massBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dForceBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N, N, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	glUseProgram(compute2Program);
	glUniform1i(n2ID, N);
	glUniform1f(dt2ID, dt);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dForceBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Simulation2::draw(){
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, massBuffer);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*) 0);
	glDrawArrays(GL_POINTS, 0, N);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);
}

Simulation2::~Simulation2(){
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
}