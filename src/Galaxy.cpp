#include "Galaxy.hpp"

Galaxy::Galaxy(){
	N = 0;
}

Galaxy::Galaxy(int N){
	this->N = N;
	stars = std::vector<Star>(N);
}