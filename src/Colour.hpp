#ifndef COLOUR_HPP
#define COLOUR_HPP

#include <glm/glm.hpp>
#include <cmath>

//Function for converting star temperature to RGBA
//Source: tannerhelland.com/4435/convert-temperature-rgb-algorithm-code/
glm::vec4 getColour(float T);

#endif