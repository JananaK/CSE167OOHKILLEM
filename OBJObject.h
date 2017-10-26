//#pragma once

#ifndef _OBJOBJECT_H_
#define _OBJOBJECT_H_

#define GLFW_INCLUDE_GLEXT
#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>
// Use of degrees is deprecated. Use radians instead.
#ifndef GLM_FORCE_RADIANS
#define GLM_FORCE_RADIANS
#endif

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stdio.h>
#include <iostream>

class OBJObject
{
// Private variables copied from Project 1
private:
	std::vector<GLuint> indicesOBJ;
	std::vector<glm::vec3> verticesOBJ;
	std::vector<glm::vec3> normalsOBJ;
	glm::mat4 originalWorld;
	float angle;

public:
	OBJObject(const char* filepath);
	~OBJObject();

	glm::mat4 toWorld;

	void parse(const char* filepath);	// Taken from P1
	void draw(GLuint);
	void translate(glm::vec3 translateVector);
	void scale(glm::vec3 scaleVector);
	void update();
	void spin(float);
	void centerScale();

	// These variables are needed for the shader program
	GLuint VBO, VAO, EBO;
	GLuint uProjection, uModelview;
};

#endif

