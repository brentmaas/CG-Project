#include "Star.hpp"

#include <iostream>
#include <cmath>

const float E = 2.71828182846f;

const float T_SUN = 5772.005317; //= pow(L_SUN / (4 * PI * SIGMA * R_SUN * R_SUN), 0.25)
const float tau = 500; //Characteristic stellar lifetime
//const float s3factor = (pow(E, 0.1) - 1) / pow(0.1, 3);

Star::Star(float m, float R, float g):
	m(m), R(pow(m / rhoC, 1.0f / 3.0f)), R0(R), age(0), g(g), tC(sqrt(R0*R0 / (acc * g * m) * (R0 - pow(m / rhoC, 1.0f / 3.0f)))),
	tC2(2 * tC + 1), stage(0){
	
}

void Star::setMR(float m, float R){
	this->m = m;
	//this->R = R;
	this->R = pow(m / rhoC, 1.0f / 3.0f);
	R0 = R;
	age = 0;
	//tC = sqrt(R0*R0 / (acc * g * m) * (R0 - pow(m / rhoC, 1.0f / 3.0f)));
	tC = sqrt(R0 / g / m);
	tC2 = tC + 1 + tau * pow(m, -2.5);
}

void Star::update(float dt){
	age += dt;
	if(stage == 0 && age > tC) stage++;
	if(stage == 1 && age > tC + 0.1) stage++;
	if(stage == 2 && age > tC2) stage++;
}

//en.wikipedia.org/wiki/Mass%E2%80%93luminosity_relation
float Star::L(){
	float L;
	if(m < 0.43) L = 0.23 * pow(m, 2.3);
	else if(m < 2) L = pow(m, 4);
	else if(m < 20) L = 1.4 * pow(m, 3.5);
	else L = 32000 * m;
	if(stage == 1) L *= (age - tC) / 0.1;
	//Vind ff een goeie functie hiervoor (exp omhoog, exp omlaag?)
	/*if(stage == 3 && m >= 8 && age < tC2 + 0.1) L *= (10 - 900 * (age - tC2 - 0.1) * (age - tC2 - 0.1));
	if(stage == 3 && m >= 8){
		L *= 10 * pow(E, -10 * (age - tC2 - 0.1));
	}*/
	//if(stage == 3 && m >= 8 && age > tC2 + 0.1) L *= s3factor * pow(age - tC2, 3) / (pow(E, age - tC2) - 1);
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