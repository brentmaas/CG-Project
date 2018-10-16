#include "Simulation.hpp"

#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

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

GLuint generateProgram(const char* computeFile){
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

Simulation::Simulation(){
	N = 0;
	g = 1;
	hr = 1;
	hz = 1;
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

Simulation::Simulation(int N, float g, float hr, float hz, int seed){
	this->N = N;
	this->g = g;
	this->hr = hr;
	this->hz = hz;
	srand(seed);
	dist = DistributionDisk(seed);
	dist.setH(this->hr, this->hz);
	xParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	vParticles = std::vector<glm::vec4>(N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
	dForce = std::vector<glm::vec4>(N * N, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
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

void Simulation::update(float dt){
	glUseProgram(computeProgram);
	glUniform1i(nID, N);
	glUniform1f(gID, g);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, massBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dForceBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N, N, 1);
	
	glUseProgram(compute2Program);
	glUniform1i(n2ID, N);
	glUniform1f(dt2ID, dt);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertexBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, velocityBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, dForceBuffer);
	
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glDispatchCompute(N, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	//Energy check
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glm::vec4 x[N], v[N];
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(glm::vec4), x);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size() * sizeof(glm::vec4), v);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	
	glm::vec3 mmp = glm::vec3(0, 0, 0);
	float totmass = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x2 = x[i];
		float m = mass[i];
		mmp += glm::vec3(m * x2.x, m * x2.y, m * x2.z);
		totmass += m;
	}
	mmp /= totmass;
	
	float energy = 0;
	for(int i = 0;i < N;i++){
		glm::vec4& x2 = x[i], v2 = v[i];
		float r2 = (x2.x - mmp.x) * (x2.x - mmp.x) + (x2.y - mmp.y) * (x2.y - mmp.y) + (x2.z - mmp.z) * (x2.z - mmp.z);
		float vel2 = v2.x * v2.x + v2.y * v2.y + v2.z * v2.z;
		energy += mass[i] * vel2 / 2 - g * mass[i] * totmass / sqrt(r2);
	}
	
	std::cout << energy << std::endl;
}

void Simulation::draw(){
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

Simulation::~Simulation(){
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
}