#ifndef COLOR_HPP
#define COLOR_HPP

#include <glm/glm.hpp>
#include <cmath>

//http://www.tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
glm::vec4 getColor(float T){
	glm::vec4 c = glm::vec4(0, 0, 0, 1);
	
	//R
	if(T < 6600) c.x = 1;
	else{
		c.x = 329.698727446 * pow(T / 100 - 60, -0.1332047592) / 256;
		if(c.x > 1) c.x = 1;
		if(c.x < 0) c.x = 0;
	}
	
	//G
	if(T < 6600){
		c.y = (99.4708025861 * log(T / 100) - 161.1195681661) / 256;
		if(c.y > 1) c.y = 1;
		if(c.y < 0) c.y = 0;
	}else{
		c.y = 288.1221695283 / 256 * pow(T / 100 - 60, -0.0755148492);
		if(c.y > 1) c.y = 1;
		if(c.y < 0) c.y = 0;
	}
	
	//B
	if(T < 2000) c.z = 0;
	else if(T > 6500) c.z = 1;
	else{
		c.z = (138.5177312231 * log(T / 100 - 10) - 305.0447927307) / 256;
		if(c.z > 1) c.z = 1;
		if(c.z < 0) c.z = 0;
	}
	
	return c;
}

#endif COLOR_HPP