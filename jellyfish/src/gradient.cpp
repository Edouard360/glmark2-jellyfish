#include "gradient.h"
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>

#include <fstream>
using namespace std;

Gradient::Gradient()
{
}

Gradient::~Gradient()
{
}

void
Gradient::createAndFillBuffers()
{
	static const GLfloat vertices[] = {
		-1.0f,-1.0f,-1.0f, 1.0f,1.0f, -1.0f, 1.0f, 1.0f
	};
	static const GLfloat uvIn[] = {
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	GLuint uvBuffer;
	glGenBuffers(1, &uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvIn), uvIn, GL_STATIC_DRAW);

	bufferObjects_[0] = vertexbuffer;
	bufferObjects_[1] = uvBuffer;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
Gradient::getUniformLocation()
{
	color1Location = glGetUniformLocation(programID, "color1");
	color2Location = glGetUniformLocation(programID, "color2");
}

void
Gradient::initializeUniforms()
{
	color1 = glm::vec3(0.360784314, 0.584313725, 1.0); //light blue
	color2 = glm::vec3(0.074509804, 0.156862745, 0.619607843); //dark blue
}

void
Gradient::bindUniforms()
{
	glUniform3fv(color1Location,1,&color1[0]);
	glUniform3fv(color2Location, 1, &color2[0]);
}

void
Gradient::vertexAttribPointer()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[0]);
	glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0, (void*)0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[1]);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,  0,(void*)0);
}

void
Gradient::disableAttributes() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

//PUBLIC METHODS
void
Gradient::initialize()
{
	programID = LoadShaders("shaders/gradient.vert", "shaders/gradient.frag");
	createAndFillBuffers();
	getUniformLocation();
	initializeUniforms();
	return;
}

void
Gradient::draw()
{
	glUseProgram(programID);
	bindUniforms();
	vertexAttribPointer();

	// Draw the triangles !
	glDrawArrays(GL_TRIANGLE_STRIP,0,4);

	disableAttributes();
}

void
Gradient::cleanup()
{
	glDeleteBuffers(1, &bufferObjects_[0]);
	glDeleteBuffers(1, &bufferObjects_[1]);
	glDeleteProgram(programID);
}
