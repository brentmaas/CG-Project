#include "Cloud.hpp"

//https://learnopengl.com/Advanced-OpenGL/Framebuffers

Cloud::Cloud(int planes):
	texs(planes), planes(planes), vertexBufferID(0), fbos(planes){
	rebuildPlanes();
}

void Cloud::draw(){
	
}

void Cloud::setPlanes(int planes){
	this->planes = planes;
	for(int i = 0;i < texs.size();i++) glDeleteTextures(1, &texs[i]);
	for(int i = 0;i < fbos.size();i++) glDeleteFramebuffers(1, &fbos[i]);
	rebuildPlanes();
}

Cloud::~Cloud(){
	glDeleteBuffers(1, &vertexBufferID);
	for(int i = 0;i < fbos.size();i++) glDeleteFramebuffers(1, &fbos[i]);
}

void Cloud::rebuildPlanes(){
	texs.reserve(planes);
	fbos.reserve(planes);
	for(int i = 0;i < planes;i++){
		glGenTextures(1, &texs[i]);
		glGenFramebuffers(1, &fbos[i]);
	}
}