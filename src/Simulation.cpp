#include "Simulation.hpp"
#include <cmath>
#include <iostream>

const float PI  = 3.141592f;

Simulation::Simulation(){
	N = 0;
	g = 1;
	vertexBuffer = 0;
	colorBuffer = 0;
	massBuffer = 0;
}

Simulation::Simulation(int N, float g, int seed){
	srand(seed);
	this->N = N;
	this->g = g;
	xParticles = std::vector<float>(3 * N);
	vParticles = std::vector<float>(3 * N, 0.0f);
	std::vector<float> colorBufferData(4 * N, 1.0f);
	mass = std::vector<float>(N, 1.0f);
	for(int i = 0;i < N;i++){
		float p = 2 * PI * ((float) rand()) / RAND_MAX;
		float t = PI * ((float) rand()) / RAND_MAX;
		float vp = 2 * PI * ((float) rand()) / RAND_MAX;
		float vt = PI * ((float) rand()) / RAND_MAX;
		xParticles[3 * i] = 5 * cos(p) * sin(t);
		xParticles[3 * i + 1] = 5 * sin(p) * sin(t);
		xParticles[3 * i + 2] = 5 * cos(t);
		//vParticles[3 * i] = cos(vp) * sin(vt);
		//vParticles[3 * i + 1] = sin(vp) * sin(vt);
		//vParticles[3 * i + 2] = cos(vt);
		colorBufferData[4 * i] = ((float) rand()) / RAND_MAX;
		colorBufferData[4 * i + 1] = ((float) rand()) / RAND_MAX;
		colorBufferData[4 * i + 2] = ((float) rand()) / RAND_MAX;
	}
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, xParticles.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, xParticles.size() * sizeof(GLfloat), xParticles.data());
	
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(GLfloat), colorBufferData.data(), GL_STATIC_DRAW);
	
	
	glGenBuffers(1, &massBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, massBuffer);
	glBufferData(GL_ARRAY_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
}

void Simulation::update(float dt){
	for(int i = 0;i < N;i++){
		for(int j = 0;j < N;j++) if(i != j && mass[i] != 0){
			float dx = xParticles[3 * i] - xParticles[3 * j];
			float dy = xParticles[3 * i + 1] - xParticles[3 * j + 1];
			float dz = xParticles[3 * i + 2] - xParticles[3 * j + 2];
			float d = sqrt(dx * dx + dy * dy + dz * dz);
			
			if(d <= 0.005f){
				vParticles[3 * i] += mass[j] * vParticles[3 * j] / mass[i];
				vParticles[3 * i + 1] += mass[j] * vParticles[3 * j + 1] / mass[i];
				vParticles[3 * i + 2] += mass[j] * vParticles[3 * j + 2] / mass[i];
				mass[i] += mass[j];
				mass[j] = 0;
			}else{
				vParticles[3 * i] -= g * mass[j] * dx / pow(d, 3) * dt;
				vParticles[3 * i + 1] -= g * mass[j] * dy / pow(d, 3) * dt;
				vParticles[3 * i + 2] -= g * mass[j] * dz / pow(d, 3) * dt;
			}
		}
		xParticles[3 * i] += vParticles[3 * i] * dt;
		xParticles[3 * i + 1] += vParticles[3 * i + 1] * dt;
		xParticles[3 * i + 2] += vParticles[3 * i + 2] * dt;
	}
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, xParticles.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, xParticles.size() * sizeof(GLfloat), xParticles.data());
	
	glBindBuffer(GL_ARRAY_BUFFER, massBuffer);
	glBufferData(GL_ARRAY_BUFFER, mass.size() * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, mass.size() * sizeof(GLfloat), mass.data());
}

void Simulation::draw(){
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

Simulation::~Simulation(){
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &colorBuffer);
}