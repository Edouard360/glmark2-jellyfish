
#ifndef SCENE_JELLYFISH_
#define SCENE_JELLYFISH_

//THÉORIQUEMENT LE MINIMUM ICI
// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "../util/time.h"
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW ?? En a-t-on besoin ?
//#include <glfw3.h>

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

class Jellyfish
{
	void createAndFillBuffers(
		std::vector<GLfloat> vertex,
		std::vector<GLfloat> normals,
		std::vector<GLfloat> colors,
		std::vector<GLfloat> texture,
		std::vector<GLuint> faces
	);
    void getUniformLocation();
    void initializeUniforms();
	void initializeTextures();
    void updateUniforms();
    void bindUniforms();
    void vertexAttribPointer();
	void disableAttributes();

    GLuint bufferObjects_[5];
    GLuint uSampler[33];
    unsigned int whichCaustic_;

    GLuint programID;

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
	float rotation;
	int whichCaustic;

	glm::mat4 uWorld;
	glm::mat4 uWorldViewProj;
	glm::mat4 uWorldInvTranspose;

	GLuint uWorldLocation;
	GLuint uWorldViewProjLocation;
	GLuint uWorldInvTransposeLocation;

	GLuint uLightPosLocation;
	GLuint uLightRadiusLocation;
	GLuint uLightColLocation;
	GLuint uAmbientColLocation;
	GLuint uFresnelColLocation;
	GLuint uFresnelPowerLocation;
	GLuint uCurrentTimeLocation;
	
	GLuint uSamplerLocation;
	GLuint uSampler1Location;

	int countForFPS;
	uint64_t accumulatedTimeInMs;
	int AverageFPS;
	glm::vec3 offset;

public:
	Jellyfish();
    Jellyfish(glm::vec3 offset);
    ~Jellyfish();
    void initialize();
    void cleanup();
    void draw();
};

class JellyfishGroup {
	std::vector<Jellyfish* > group;

public:
	JellyfishGroup();
	~JellyfishGroup();
	void initialize();
	void cleanup();
	void draw();
};

class JellyfishInstanced
{
	void createAndFillBuffers(
		std::vector<GLfloat> vertex,
		std::vector<GLfloat> normals,
		std::vector<GLfloat> colors,
		std::vector<GLfloat> texture,
		std::vector<GLfloat> offset,
		std::vector<GLuint> faces
	);
	void parseOffset(std::vector<GLfloat>& offset);
	void getUniformLocation();
	void initializeUniforms();
	void initializeTextures();
	void updateUniforms();
	void bindUniforms();
	void vertexAttribPointer();
	void disableAttributes();

	GLuint bufferObjects_[6];
	GLuint uSampler[33];
	unsigned int whichCaustic_;

	GLuint programID;

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
	float rotation;
	int whichCaustic;

	glm::mat4 uWorld;
	glm::mat4 uWorldViewProj;
	glm::mat4 uWorldInvTranspose;

	GLuint uWorldLocation;
	GLuint uWorldViewProjLocation;
	GLuint uWorldInvTransposeLocation;

	GLuint uLightPosLocation;
	GLuint uLightRadiusLocation;
	GLuint uLightColLocation;
	GLuint uAmbientColLocation;
	GLuint uFresnelColLocation;
	GLuint uFresnelPowerLocation;
	GLuint uCurrentTimeLocation;

	GLuint uSamplerLocation;
	GLuint uSampler1Location;

	int numberOfJellyfish;
	int countForFPS;
	uint64_t accumulatedTimeInMs;
	int AverageFPS;

public:
	JellyfishInstanced();
	~JellyfishInstanced();
	void initialize();
	void cleanup();
	void draw();
};

#endif
