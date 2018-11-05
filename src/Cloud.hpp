#ifndef CLOUD_HPP
#define CLOUD_HPP

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Cloud{
public:
	std::vector<GLuint> texs;
	
	Cloud(int planes);
	void draw();
	void setPlanes(int planes);
	~Cloud();
private:
	std::vector<GLuint> fbos;
	int planes;
	GLuint vertexBufferID;
	
	void rebuildPlanes();
};

#endif