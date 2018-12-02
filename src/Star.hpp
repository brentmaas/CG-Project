#ifndef STAR_HPP
#define STAR_HPP

class Star{
public:
	//Star with mass m, under gravitational constant g
	Star(float m, float g);
	//Set mass and radius
	void setMR(float m, float R);
	void update(float dt);
	void reset();
	//Luminosity
	float L();
	//Temperature
	float T();
	void addAge(float dage);
	void setAge(float age);
	void setStage(int stage);
	float getM(); //Get mass
	float getR(); //Get radius
	float getTC(); //Get collapse time
	float getTC2(); //Get stellar lifetime
	float getAge();
	int getStage();
private:
	//Mass, radius, age and gravitation
	float m, R, age, g; //m, R in solar units
	float tC, tC2; //Collapse time, stellar lifetime
	//stage: 0: collapsing gas cloud, 1: protostar, 2: star, 3: dying star (only nova)
	int stage;
};

#endif