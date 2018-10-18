#ifndef GALAXY_HPP
#define GALAXY_HPP

#include <vector>
#include <GL/glew.h>
#include "Colour.hpp"
#include "Star.hpp"

class Galaxy{
public:
	Galaxy();
	Galaxy(int N);
	void buildColorBuffer(GLuint& colorBufferID);
private:
	int N;
	std::vector<Star> stars;
};

#endif