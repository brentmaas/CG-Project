#ifndef STAR_HPP
#define STAR_HPP

const float rhoC = 0.01f, acc = 1;

class Star{
public:
	Star(float m, float R, float g);
	void setMR(float m, float R);
	void update(float dt);
	float L();
	float T();
	float getM();
	float getR();
	int getStage();
private:
	float m, R, R0, age, tC, g; //m, R in solar units
	int stage;
};

#endif