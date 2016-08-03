#ifndef ABSTRACT_JELLYFISH_
#define ABSTRACT_JELLYFISH_

// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../util/time.h"
#include <vector>
#include <map>

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

// Include fstream and string
#include <fstream>
#include <string>
using namespace std;

// Include Texture and Shader utilities
#include <common/shader.hpp>
#include <common/texture.hpp>

struct uniform {
	GLuint location;
};

struct attribute {
	GLuint location;
	GLuint buffer;
	vector<GLfloat> input;
};

class AbstractJellyfish
{
protected:
	virtual void createProgram();
	virtual void draw();
	virtual void insertAttributes();
	void createAndFillBuffers();
	void insertUniforms();
	void getUniformLocation();
	void getAttribLocation();
	void initializeUniforms();
	void initializeTextures();
	void updateUniforms();
	void bindUniforms();
	virtual void vertexAttribPointer();

	GLuint programID;

	std::map<string, uniform> uniformMap;
	std::map<string, attribute> attributeMap;
	GLuint indexBuffer;
	GLuint elementCount;

	GLuint uSampler[33];
	int whichCaustic;
	float rotation;

	glm::mat4 uWorld;
	glm::mat4 uWorldViewProj;
	glm::mat4 uWorldInvTranspose;

	glm::vec3 uLightPos;
	float uLightRadius;
	glm::vec4 uLightCol;
	glm::vec4 uAmbientCol;
	glm::vec4 uFresnelCol;
	float uFresnelPower;
	float uCurrentTime;

	struct timeval tv;
	uint64_t lastUpdateTime_;
	uint64_t now;
	uint64_t accumulatedTimeInMs;
	int countForFPS;
	int AverageFPS;

public:
	AbstractJellyfish();
    ~AbstractJellyfish();
    void initialize();
    void render();
	virtual void cleanup();
};

#endif
