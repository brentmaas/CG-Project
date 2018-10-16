#include "SimulationSimple.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

const float PI  = 3.141592f;
GLuint loadComputeShader2(const char* file, GLuint type){
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

GLuint generateProgram2(const char* computeFile){
	GLuint computeShaderID = loadComputeShader2(computeFile, GL_COMPUTE_SHADER);
	
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

SimulationSimple::SimulationSimple(){
	N = 0;
	g = 1;
	hr = 1;
	hz = 1;
	mID = 0;
	gID = 0;
	mmpID = 0;
	dtID = 0;
	vertexBuffer = 0;
	colorBuffer = 0;
	massBuffer = 0;
	velocityBuffer = 0;
	computeProgram = 0;
}

SimulationSimple::SimulationSimple(int N, float g, float hr, float hz, int seed){
	this->N = N;
	this->g = g;
	this->hr = hr;
	this->hz = hz;
	srand(seed);
	dist = DistributionDisk(seed);
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
		c.x = ((float) rand()) / RAND_MAX;
		c.y = ((float) rand()) / RAND_MAX;
		c.z = ((float) rand()) / RAND_MAX;
	}
	
	glm::vec3 mmp = glm::vec3(0, 0, 0);
	float totmass = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x = xParticles[i];
		float m = mass[i];
		mmp += glm::vec3(m * x.x, m * x.y, m * x.z);
		totmass += m;
	}
	mmp /= totmass;
	for(int i = 0;i < N;i++){
		glm::vec4& x = xParticles[i];
		float r = sqrt((x.x - mmp.x) * (x.x - mmp.x) + (x.y - mmp.y) * (x.y - mmp.y) + (x.z - mmp.z) * (x.z - mmp.z));
		glm::vec4& v = vParticles[i];
		if(r <= 0){
			v = glm::vec4(0, 0, 0, 0);
			continue;
		}
		float vtot = sqrt(g * totmass / r);
		float rproj = sqrt((x.x - mmp.x) * (x.x - mmp.x) + (x.z - mmp.z) * (x.z - mmp.z));
		float costheta = glm::dot(glm::vec3((x.x - mmp.x) / r, (x.y - mmp.y) / r, (x.z - mmp.z) / r), glm::vec3((x.x - mmp.x) / rproj, 0, (x.z - mmp.z) / rproj));
		float vproj = vtot * costheta;
		v.x = vproj * (x.z - mmp.z) / rproj;
		v.y = vtot * sqrt(std::max(0.0f, 1 - costheta * costheta));
		v.z = -vproj * (x.x - mmp.x) / rproj;
	}
	
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), xParticles.data());
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colorBufferData.size() * sizeof(glm::vec4), colorBufferData.data());
	
	computeProgram = generateProgram2("resources/shaders/particles_simple.compute");
	mID = glGetUniformLocation(computeProgram, "m");
	gID = glGetUniformLocation(computeProgram, "g");
	mmpID = glGetUniformLocation(computeProgram, "mmp");
	dtID = glGetUniformLocation(computeProgram, "dt");
	
	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(glm::vec4), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), vParticles.data());
	
	glGenBuffers(1, &massBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, massBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
	
	glm::vec3 mmp2 = glm::vec3(0, 0, 0);
	float totmass2 = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x2 = xParticles[i];
		float m = mass[i];
		mmp2 += glm::vec3(m * x2.x, m * x2.y, m * x2.z);
		totmass2 += m;
	}
	mmp2 /= totmass2;
	
	float energy = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x2 = xParticles[i], v2 = vParticles[i];
		//std::cout << "x: " << x2.x << " " << x2.y << " " << x2.z << " " << x2.w << std::endl;
		//std::cout << "v: " << v2.x << " " << v2.y << " " << v2.z << " " << v2.w << std::endl;
		float r2 = (x2.x - mmp2.x) * (x2.x - mmp2.x) + (x2.y - mmp2.y) * (x2.y - mmp2.y) + (x2.z - mmp2.z) * (x2.z - mmp2.z);
		float vel2 = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
		energy += mass[i] * vel2 / 2 - g * mass[i] * totmass2 / sqrt(r2);
	}
	
	std::cout << "Starting energy: " << energy << std::endl;
	//exit(0);
}

void SimulationSimple::update(float dt){
	glm::vec4 mmp = glm::vec4(0, 0, 0, 1);
		float totmass = 0;
		for(int i = 0;i < N;i++){
			glm::vec4& x2 = xParticles[i];
			float m = mass[i];
			mmp += glm::vec4(m * x2.x, m * x2.y, m * x2.z, 0);
			totmass += m;
		}
		mmp /= totmass;
	glUseProgram(computeProgram);
	glUniform1f(mID, totmass);
	glUniform1f(gID, g);
	glUniform1fv(mmpID, 1, &mmp[0]);
	glUniform1f(dtID, dt);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N, 1, 1);
	
	//Energy check
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glm::vec4 x[N], v[N];
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), x);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), v);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	glm::vec3 mmp2 = glm::vec3(0, 0, 0);
	float totmass2 = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x2 = x[i];
		float m = mass[i];
		mmp2 += glm::vec3(m * x2.x, m * x2.y, m * x2.z);
		totmass2 += m;
	}
	mmp2 /= totmass2;
	
	float energy = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x2 = x[i], v2 = v[i];
		float r2 = (x2.x - mmp2.x) * (x2.x - mmp2.x) + (x2.y - mmp2.y) * (x2.y - mmp2.y) + (x2.z - mmp2.z) * (x2.z - mmp2.z);
		float vel2 = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
		energy += mass[i] * vel2 / 2 - g * mass[i] * totmass2 / sqrt(r2);
	}
	
	std::cout << energy << std::endl;
}

void SimulationSimple::draw(){
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

SimulationSimple::~SimulationSimple(){
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
}