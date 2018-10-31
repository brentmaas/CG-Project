#include "Star.hpp"

#include <cmath>

const float T_SUN = 5772.005317; //= pow(L_SUN / (4 * PI * SIGMA * R_SUN * R_SUN), 0.25)

Star::Star(float m, float R):
	m(m), R(R), R0(R), age(0){
	
}

void Star::setMR(float m, float R){
	this->m = m;
	this->R = R;
	R0 = R;
	age = 0;
}

void Star::update(float dt){
	age += dt;
}

//en.wikipedia.org/wiki/Mass%E2%80%93luminosity_relation
float Star::L(){
	float L;
	if(m < 0.43) L = 0.23 * pow(m, 2.3);
	else if(m < 2) L = pow(m, 4);
	else if(m < 20) L = 1.4 * pow(m, 3.5);
	else L = 32000 * m;
	return L;
}

float Star::T(){
	return T_SUN * pow(L() / (R * R), 0.25);
}

float Star::getM(){
	return m;
}

float Star::getR(){
	return R;
}