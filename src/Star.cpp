#include "Star.hpp"

#include <iostream>
#include <cmath>

const float E = 2.71828182846f;

const float T_SUN = 5772.005317; //= pow(L_SUN / (4 * PI * SIGMA * R_SUN * R_SUN), 0.25)
const float tau = 500; //Characteristic stellar lifetime
const float ts1 = 0.1; //Time in stage 1
//Supernova constants: minimal mass, amplification, decay, time to maximum, time at maximum
const float snLim = 9.5, snAmp = 1000, snDecay = 30, snT1 = 0.005, snT2 = 0.025;

Star::Star(float m, float R, float g):
	m(m), R(pow(m, 1.0f / 3.0f)), age(0), g(g), tC(1),
	tC2(3), stage(0){
	
}

void Star::setMR(float m, float R){
	this->m = m;
	this->R = pow(m, 1.0f / 3.0f);
	age = 0;
	tC = sqrt(R / g / m);
	tC2 = tC + 1 + tau * pow(m, -2.5);
}

void Star::update(float dt){
	age += dt;
	if(stage == 0 && age > tC) stage++;
	if(stage == 1 && age > tC + ts1) stage++;
	if(stage == 2 && age > tC2) stage++;
	if(stage == 3 && m >= snLim && L() < 200) reset();
}

void Star::reset(){
	age = 0;
	stage = 0;
}

//en.wikipedia.org/wiki/Mass%E2%80%93luminosity_relation
float Star::L(){
	if(stage == 0) return 0;
	float L;
	if(m < 0.43) L = 0.23 * pow(m, 2.3);
	else if(m < 2) L = pow(m, 4);
	else if(m < 20) L = 1.4 * pow(m, 3.5);
	else L = 32000 * m;
	if(stage == 1) L *= (age - tC) / ts1;
	if(stage == 3 && m >= snLim && age < tC2 + snT1) L *= pow(E, snAmp * (age - tC2));
	else if(stage == 3 && m >= snLim && age < tC2 + snT1 + snT2) L *= pow(E, snAmp * snT1);
	else if(stage == 3 && m >= snLim) L *= pow(E, snAmp * snT1 - snDecay * (age - tC2 - snT1 - snT2));
	return L;
}

float Star::T(){
	return T_SUN * pow(L() / (R * R), 0.25);
}

void Star::addAge(float dage){
	age += dage;
}

void Star::setAge(float age){
	this->age = age;
}

void Star::setStage(int stage){
	this->stage = stage;
}

float Star::getM(){
	return m;
}

float Star::getR(){
	return R;
}

float Star::getTC(){
	return tC;
}

float Star::getTC2(){
	return tC2;
}

float Star::getAge(){
	return age;
}

int Star::getStage(){
	return stage;
}