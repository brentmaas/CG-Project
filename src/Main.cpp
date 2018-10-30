#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <thread>

#include "SimulationSimple.hpp"
//#include "Galaxy.hpp"

const std::string title = "CG Project Brent Maas";
const float PI = 3.14159265359f;
const int width = 1000, height = 600;
const float targetFPS = 60.0f;

GLuint loadShader(const char* file, GLuint type){
	GLuint shaderID = glCreateShader(type);
	
	std::string shaderSrc;
	std::ifstream shaderStream(file, std::ios::in);
	if(shaderStream.is_open()){
		std::stringstream strStream;
		strStream << shaderStream.rdbuf();
		shaderSrc = strStream.str();
		shaderStream.close();
	}else{
		std::cerr << "Could not open" << file << std::endl;
		getchar();
		return 0;
	}
	
	char const * srcPtr = shaderSrc.c_str();
	glShaderSource(shaderID, 1, &srcPtr, NULL);
	glCompileShader(shaderID);
	
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0){
		std::vector<char> errMsg(infoLogLength + 1);
		glGetShaderInfoLog(shaderID, infoLogLength, NULL, &errMsg[0]);
		std::cerr << &errMsg[0] << std::endl;
	}
	
	return shaderID;
}

GLuint generateProgram(const char* vertexFile, const char* fragmentFile){
	GLuint vertexShaderID = loadShader(vertexFile, GL_VERTEX_SHADER);
	GLuint fragmentShaderID = loadShader(fragmentFile, GL_FRAGMENT_SHADER);
	
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);
	
	GLint result = GL_FALSE;
	int infoLogLength;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if(infoLogLength > 0){
		std::vector<char> errMsg(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errMsg[0]);
		std::cerr << &errMsg[0] << std::endl;
	}
	
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);
	
	return programID;
}

int main(int argc, char **argv){
	if(!glfwInit()){
		std::cerr << "Could not initialise GLFW." << std::endl;
		return 1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	
	GLFWwindow* window;
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if(window == NULL){
		std::cerr << "Could not create window." << std::endl;
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	if(glewInit() != GLEW_OK){
		std::cerr << "Could not initialise GLEW." << std::endl;
		glfwTerminate();
		return 1;
	}
	
	//GLuint programDefault = generateProgram("resources/shaders/default.vertex", "resources/shaders/default.fragment");
	//GLuint programInstanced = generateProgram("resources/shaders/instanced.vertex", "resources/shaders/instanced.fragment");
	GLuint programParticles = generateProgram("resources/shaders/particles.vertex", "resources/shaders/particles.fragment");
	
	glUseProgram(programParticles);
	
	glm::mat4 projection = glm::perspective(45.0f, ((float) width) / height, 0.01f, 100000.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 500.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = projection * view * model;
	//GLuint defaultMatrixID = glGetUniformLocation(programDefault, "MVP");
	//glUniformMatrix4fv(defaultMatrixID, 1, GL_FALSE, &mvp[0][0]);
	//GLuint instancedMatrixID = glGetUniformLocation(programInstanced, "MVP");
	//glUniformMatrix4fv(instancedMatrixID, 1, GL_FALSE, &mvp[0][0]);
	GLuint particlesMatrixID = glGetUniformLocation(programParticles, "MVP");
	glUniformMatrix4fv(particlesMatrixID, 1, GL_FALSE, &mvp[0][0]);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	GLuint vertexArray/*, vertexBuffer, colorBuffer, positionBuffer*/;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	
	/*std::vector<float> vertexBufferData = {-0.7f, 0.0f, 0.0f, 0.7f, 0.0f, 0.0f, 0.0f, 0.7f, 0.0f};
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertexBufferData.size() * sizeof(GLfloat), vertexBufferData.data(), GL_STATIC_DRAW);
	
	std::vector<float> colorBufferData = {1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f};
	glGenBuffers(1, &colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, colorBufferData.size() * sizeof(GLfloat), colorBufferData.data(), GL_STATIC_DRAW);
	
	std::vector<float> positionBufferData = {2.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, positionBufferData.size() * sizeof(GLfloat), positionBufferData.data(), GL_STATIC_DRAW);*/
	
	float ang = 0, ang2 = PI / 2;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	auto now = std::chrono::high_resolution_clock::now();
	
	SimulationSimple sim(50000, 1.0f, 50.0f, 10.0f, std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
	//Galaxy galaxy(50000, 1.0f, 50.0f, 10.0f, std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
	
	while(!glfwWindowShouldClose(window)){
		auto now2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> d = now2 - now;
		float dt = d.count();
		now = now2;
		
		//std::cout << 1.0f / dt << std::endl;
		glfwSetWindowTitle(window, (title + " - " + std::to_string((int) (1.0f / dt)) + " fps").c_str());
		
		sim.update(dt / 10.0f);
		//galaxy.sim.update(dt / 10.0f);
		glUseProgram(programParticles);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) ang += 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) ang -= 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) ang2 -= 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) ang2 += 0.5f * dt;
		if(ang2 > PI / 2) ang2 = PI / 2;
		if(ang2 < -PI / 2) ang2 = -PI / 2;
		glm::mat4 mat = mvp * glm::rotate(glm::mat4(1.0f), ang2, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), ang, glm::vec3(0, 1, 0));
		
		glUniformMatrix4fv(particlesMatrixID, 1, GL_FALSE, &mat[0][0]);
		
		/*glUseProgram(programInstanced);
		glUniformMatrix4fv(instancedMatrixID, 1, GL_FALSE, &mat[0][0]);
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 0);
		glVertexAttribDivisor(2, 1);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 3, 2);
		glDisableVertexAttribArray(2);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);
		
		glUseProgram(programDefault);
		glUniformMatrix4fv(defaultMatrixID, 1, GL_FALSE, &mat[0][0]);
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(0);*/
		sim.draw();
		//galaxy.sim.draw();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}