#ifndef STAR_HPP
#define STAR_HPP

class Star{
public:
	Star();
	Star(float m, float R);
	float T();
	float getM();
private:
	float m, R, age; //m, R in solar units
};

#endif