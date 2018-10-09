#include "Simulation2.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>

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
	//srand(seed);
	dist = DistributionDisk(seed);
	dist.setH(5, 1);
	this->N = N;
	this->g = g;
	xParticles = std::vector<float>(3 * N, 0.0f);
	vParticles = std::vector<float>(3 * N, 0.0f);
	dForce = std::vector<float>(3 * N * N, 0.0f);
	colorBufferData = std::vector<float>(4 * N, 1.0f);
	mass = std::vector<float>(N, 5.0f);
	for(int i = 0;i < N;i++){
		//float p = 2 * PI * ((float) rand()) / RAND_MAX;
		//float t = PI * ((float) rand()) / RAND_MAX;
		//float vp = 2 * PI * ((float) rand()) / RAND_MAX;
		//float vp = p + PI / 2;
		//float vt = PI * ((float) rand()) / RAND_MAX;
		//float vt = PI / 2;
		//xParticles[3 * i] = 10 * cos(p) * sin(t);
		//xParticles[3 * i + 1] = 10 * sin(p) * sin(t);
		//xParticles[3 * i + 2] = 10 * cos(t);
		//vParticles[3 * i] = 1 * cos(vp) * sin(vt);
		//vParticles[3 * i + 1] = 1 * sin(vp) * sin(vt);
		//vParticles[3 * i + 2] = 1 * cos(vt);
		float* pos = dist.eval();
		xParticles[3 * i] = pos[0];
		xParticles[3 * i + 1] = pos[2];
		xParticles[3 * i + 2] = pos[1];
		vParticles[3 * i] = -pos[1];
		vParticles[3 * i + 1] = 0.0f;
		vParticles[3 * i + 2] = pos[0];
		colorBufferData[4 * i] = ((float) rand()) / RAND_MAX;
		colorBufferData[4 * i + 1] = ((float) rand()) / RAND_MAX;
		colorBufferData[4 * i + 2] = ((float) rand()) / RAND_MAX;
	}
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, xParticles.size() * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size() * sizeof(GLfloat), xParticles.data());
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colorBufferData.size() * sizeof(GLfloat), colorBufferData.data());
	
	computeProgram = generateProgram("resources/shaders/particles.compute");
	
	glGenBuffers(1, &velocityBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, vParticles.size() * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size(), vParticles.data());
	
	glGenBuffers(1, &massBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, massBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
	
	compute2Program = generateProgram("resources/shaders/particles.compute2");
	
	glGenBuffers(1, &dForceBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, dForceBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, dForce.size() * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, dForce.size() * sizeof(GLfloat), dForce.data());
	
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
	
	//float out[xParticles.size()];
	
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexBuffer);
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, xParticles.size(), out);
	//std::cout << out[0] << " " << xParticles[0] << std::endl;
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocityBuffer);
	//glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, vParticles.size(), vParticles.data());
	
	/*for(int i = 0;i < (int) xParticles.size() / 3;i++){
		float ax = 0, ay = 0, az = 0;
		for(int j = 0;j < (int) xParticles.size() / 3;j++) if(i != j && mass[i] != 0){
			float dx = xParticles[3 * i] - xParticles[3 * j];
			float dy = xParticles[3 * i + 1] - xParticles[3 * j + 1];
			float dz = xParticles[3 * i + 2] - xParticles[3 * j + 2];
			float d = sqrt(dx * dx + dy * dy + dz * dz);
			
			if(d <= 0.0005f * (mass[i] + mass[j])){
				vParticles[3 * i] = (mass[i] * vParticles[3 * i] + mass[j] * vParticles[3 * j]) / (mass[i] + mass[j]);
				vParticles[3 * i + 1] = (mass[i] * vParticles[3 * i + 1] + mass[j] * vParticles[3 * j + 1]) / (mass[i] + mass[j]);
				vParticles[3 * i + 2] = (mass[i] * vParticles[3 * i + 2] + mass[j] * vParticles[3 * j + 2]) / (mass[i] + mass[j]);
				mass[i] += mass[j];
				mass[j] = 0;
			}else{
				ax -= g * mass[j] * dx / (d * d * d) * dt;
				ay -= g * mass[j] * dy / (d * d * d) * dt;
				az -= g * mass[j] * dz / (d * d * d) * dt;
			}
		}
		xParticles[3 * i] += vParticles[3 * i] * dt + 0.5f * ax * dt * dt;
		xParticles[3 * i + 1] += vParticles[3 * i + 1] * dt + 0.5f * ay * dt * dt;
		xParticles[3 * i + 2] += vParticles[3 * i + 2] * dt + 0.5f * az * dt * dt;
		vParticles[3 * i] += ax * dt;
		vParticles[3 * i + 1] += ay * dt;
		vParticles[3 * i + 2] += az * dt;
	}
	
	int discard = 0;
	for(int i = 0;i < (int) mass.size();i++) if(mass[i] <= 0) discard++;
	if(discard > 0){
		std::vector<float> xParticles2(xParticles.size() - 3 * discard);
		std::vector<float> vParticles2(xParticles.size() - 3 * discard);
		std::vector<float> colorBufferData2(colorBufferData.size() - 4 * discard);
		std::vector<float> mass2(mass.size() - discard);
		int i2 = 0;
		for(int i = 0;i < (int) mass.size();i++) if(mass[i] > 0){
			xParticles2[3 * i2] = xParticles[3 * i];
			xParticles2[3 * i2 + 1] = xParticles[3 * i + 1];
			xParticles2[3 * i2 + 2] = xParticles[3 * i + 2];
			vParticles2[3 * i2] = vParticles[3 * i];
			vParticles2[3 * i2 + 1] = vParticles[3 * i + 1];
			vParticles2[3 * i2 + 2] = vParticles[3 * i + 2];
			colorBufferData2[4 * i2] = colorBufferData[4 * i];
			colorBufferData2[4 * i2 + 1] = colorBufferData[4 * i + 1];
			colorBufferData2[4 * i2 + 2] = colorBufferData[4 * i + 2];
			colorBufferData2[4 * i2 + 3] = colorBufferData[4 * i + 3];
			mass2[i2] = mass[i];
			i2++;
		}
		
		xParticles = xParticles2;
		vParticles = vParticles2;
		colorBufferData = colorBufferData2;
		mass = mass2;
		
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, colorBufferData.size() * sizeof(GLfloat), colorBufferData.data());
	}
	//std::cout << mass.size() << std::endl;
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, xParticles.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, xParticles.size() * sizeof(GLfloat), xParticles.data());
	
	glBindBuffer(GL_ARRAY_BUFFER, massBuffer);
	glBufferData(GL_ARRAY_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());*/
	
	/*float energy = 0.0f;
	for(int i = 0;i < N;i++){
		float vx = vParticles[3 * i];
		float vy = vParticles[3 * i + 1];
		float vz = vParticles[3 * i + 2];
		energy += 0.5f * mass[i] * (vx*vx + vy*vy + vz*vz);
		float x = xParticles[3 * i];
		float y = xParticles[3 * i + 1];
		float z = xParticles[3 * i + 2];
		for(int j = 0;j < N;j++) if(i != j){
			float x2 = xParticles[3 * j];
			float y2 = xParticles[3 * j + 1];
			float z2 = xParticles[3 * j + 2];
			float d = sqrt((x-x2)*(x-x2) + (y-y2)*(y-y2) + (z-z2)*(z-z2));
			energy -= g * mass[i] * mass[j] / d;
		}
	}
	std::cout << energy << std::endl;*/
}

void Simulation2::draw(){
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
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