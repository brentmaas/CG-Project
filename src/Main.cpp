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

//#include "SimulationSimple.hpp"
#include "Galaxy.hpp"

const std::string title = "CG Project Brent Maas";
const float PI = 3.14159265359f;
const int width = 1200, height = 800;
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
	
	glfwWindowHint(GLFW_SAMPLES, 8);
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
	
	GLuint programParticles = generateProgram("resources/shaders/particles.vertex", "resources/shaders/particles.fragment");
	
	glUseProgram(programParticles);
	
	glm::mat4 projection = glm::perspective(45.0f, ((float) width) / height, 0.01f, 100000.0f);
	glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 500.0f), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 mvp = projection * view * model;
	GLuint particlesMatrixID = glGetUniformLocation(programParticles, "MVP");
	glUniformMatrix4fv(particlesMatrixID, 1, GL_FALSE, &mvp[0][0]);
	
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_PROGRAM_POINT_SIZE_EXT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
	
	GLuint vertexArray;
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);
	
	float phi = 0, theta = PI / 2;
	bool play = true, spaceBlock = false;
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	auto now = std::chrono::high_resolution_clock::now();
	
	Galaxy galaxy(50000, 1.0f, 50.0f, 10.0f, std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count());
	
	while(!glfwWindowShouldClose(window)){
		auto now2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> d = now2 - now;
		float dt = d.count();
		now = now2;
		
		glfwSetWindowTitle(window, (title + " - " + std::to_string((int) (1.0f / dt)) + " fps" + (play ? "" : " - Paused")).c_str());
		
		//galaxy.update(dt / 10.0f);
		if(play) galaxy.update(0.00025f);
		glUseProgram(programParticles);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) phi += 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) phi -= 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) theta -= 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) theta += 0.5f * dt;
		if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !spaceBlock){
			play = !play;
			spaceBlock = true;
		}
		if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) spaceBlock = false;
		if(theta > PI / 2) theta = PI / 2;
		if(theta < -PI / 2) theta = -PI / 2;
		glm::mat4 mat = mvp * glm::rotate(glm::mat4(1.0f), theta, glm::vec3(1, 0, 0)) * glm::rotate(glm::mat4(1.0f), phi, glm::vec3(0, 1, 0));
		
		glUniformMatrix4fv(particlesMatrixID, 1, GL_FALSE, &mat[0][0]);
		
		galaxy.draw();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	
	return 0;
}