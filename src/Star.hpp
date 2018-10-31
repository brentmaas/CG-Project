#ifndef STAR_HPP
#define STAR_HPP

class Star{
public:
	Star(float m, float R);
	void setMR(float m, float R);
	void update(float dt);
	float L();
	float T();
	float getM();
	float getR();
private:
	float m, R, R0, age; //m, R in solar units
};

#endif