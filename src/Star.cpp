#include "Star.hpp"

#include <cmath>

const float T_SUN = 5772.005317; //= pow(L_SUN / (4 * PI * SIGMA * R_SUN * R_SUN), 0.25)

Star::Star(){
	m = 0;
	R = 0;
	age = 0;
}

Star::Star(float m, float R){
	this->m = m;
	this->R = R;
	age = 0;
}

//en.wikipedia.org/wiki/Mass%E2%80%93luminosity_relation
float Star::T(){
	float L;
	if(m < 0.43) L = 0.23 * pow(m, 2.3);
	else if(m < 2) L = pow(m, 4);
	else if(m < 20) L = 1.4 * pow(m, 3.5);
	else L = 32000 * m;
	return T_SUN * pow(L / (R * R), 0.25);
}

float Star::getM(){
	return m;
}