#ifndef GRADIENT_
#define GRADIENT_

// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

class Gradient
{
    void createAndFillBuffers();
    void getUniformLocation();
    void initializeUniforms();
    void bindUniforms();
    void vertexAttribPointer();
	void disableAttributes();

    GLuint bufferObjects_[2];

    GLuint programID;

	GLuint color1Location;
	GLuint color2Location;

    glm::vec3 color1;
	glm::vec3 color2;

public:
	Gradient();
    ~Gradient();
    void initialize();
    void cleanup();
    void draw();
};

#endif
