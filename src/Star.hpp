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
	void addAge(float dage);
	void setAge(float age);
	void setStage(int stage);
	float getM();
	float getR();
	float getTC();
	float getTC2();
	float getAge();
	int getStage();
private:
	float m, R, R0, age, g; //m, R, R0 in solar units
	float tC, tC2; //Collapse time, star lifetime
	//stage: 0: collapsing gas cloud, 1: protostar, 2: star, 3: dying star, 4: stellar remnant
	int stage;
};

#endif