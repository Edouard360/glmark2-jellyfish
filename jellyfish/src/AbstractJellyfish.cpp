#include "AbstractJellyfish.h"

// updateUniforms method needs to transform matrices
#include <glm/gtc/matrix_transform.hpp>

#include "../data/conf.h"
#include "../data/jellyfish_attributes.h"

#ifdef __APPLE__
    #include <sys/time.h>
#endif


using namespace std;

AbstractJellyfish::AbstractJellyfish():accumulatedTimeInMs(0),countForFPS(0)
{
}

AbstractJellyfish::~AbstractJellyfish()
{
}

void
AbstractJellyfish::createProgram() {
	throw "createProgram virtual method should be overridden";
}

void
AbstractJellyfish::draw() {
	throw "draw virtual method should be overridden";
}

void
AbstractJellyfish::insertAttributes() {
	std::vector<GLfloat> position(position_data, position_data + sizeof(position_data) / sizeof(position_data[0]));
	std::vector<GLfloat> normal(normal_data, normal_data + sizeof(normal_data) / sizeof(normal_data[0]));
	std::vector<GLfloat> color(color_data, color_data + sizeof(color_data) / sizeof(color_data[0]));
	std::vector<GLfloat> texture(texture_data, texture_data + sizeof(texture_data) / sizeof(texture_data[0]));

	attributeMap.insert(pair<string, attribute>("position", { 0, 0 ,position }));
	attributeMap.insert(pair<string, attribute>("normal", { 0, 0 ,normal }));
	attributeMap.insert(pair<string, attribute>("color", { 0, 0 ,color }));
	attributeMap.insert(pair<string, attribute>("texture", { 0, 0 ,texture }));
}

void
AbstractJellyfish::createAndFillBuffers()
{
	for (auto u = attributeMap.begin(); u != attributeMap.end(); u++) {
		glGenBuffers(1, &u->second.buffer);
		glBindBuffer(GL_ARRAY_BUFFER, u->second.buffer);
		glBufferData(GL_ARRAY_BUFFER, (u->second.input.size()) * sizeof(GL_FLOAT), &u->second.input[0], GL_STATIC_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void 
AbstractJellyfish::insertUniforms() {
	uniformMap.insert(pair<string, uniform>("uWorld", { 0 }));
	uniformMap.insert(pair<string, uniform>("uWorldViewProj", { 0 }));
	uniformMap.insert(pair<string, uniform>("uWorldInvTranspose", { 0 }));
	uniformMap.insert(pair<string, uniform>("uLightPos", { 0 }));
	uniformMap.insert(pair<string, uniform>("uLightRadius", { 0 }));
	uniformMap.insert(pair<string, uniform>("uLightCol", { 0 }));
	uniformMap.insert(pair<string, uniform>("uAmbientCol", { 0 }));
	uniformMap.insert(pair<string, uniform>("uFresnelCol", { 0 }));
	uniformMap.insert(pair<string, uniform>("uFresnelPower", { 0 }));
	uniformMap.insert(pair<string, uniform>("uCurrentTime", { 0 }));
	uniformMap.insert(pair<string, uniform>("uSampler", { 0 }));
	uniformMap.insert(pair<string, uniform>("uSampler1", { 0 }));
}

void
AbstractJellyfish::getUniformLocation()
{
	for (auto u = uniformMap.begin(); u != uniformMap.end(); u++) {
		u->second.location = glGetUniformLocation(programID, u->first.c_str());
	}
}

void
AbstractJellyfish::getAttribLocation()
{
	for (auto u = attributeMap.begin(); u != attributeMap.end(); u++) {
		u->second.location = glGetAttribLocation(programID, u->first.c_str());
	}
}

void
AbstractJellyfish::initializeUniforms()
{
	gettimeofday(&tv, NULL);

	now = static_cast<uint64_t>(tv.tv_sec) * 1000 + static_cast<uint64_t>(tv.tv_usec / 1000);
	lastUpdateTime_ = now;
	rotation = 0;

	uLightPos = glm::vec3(10.0, 40.0, -60.0);
	uLightRadius = 200.0;
	uLightCol = glm::vec4(0.8, 1.3, 1.1, 1.0);
	uAmbientCol = glm::vec4(0.3, 0.2, 1.0, 1.0);
	uFresnelCol = glm::vec4(0.8, 0.7, 0.6, 1.1);
	uFresnelPower = 1.0;
}

void
AbstractJellyfish::initializeTextures()
{
	int length = (sizeof(images_location_data) / sizeof(images_location_data[0]));
    
    int width,height;
    
	for (int i =   0; i < length; i++) {//CHANGE WITH ONE
		uSampler[i] = loadBMP_custom(images_location_data[i]);
    }
}

void
AbstractJellyfish::updateUniforms()
{
	gettimeofday(&tv, NULL);
	now = static_cast<uint64_t>(tv.tv_sec) * 1000 + static_cast<uint64_t>(tv.tv_usec / 1000);
    
	uint64_t elapsedTime = now - lastUpdateTime_;
	rotation += (double)(2.0 * elapsedTime) / 1000.0;
	uCurrentTime = static_cast<uint64_t>(now) % 100000000 / 1000.0;
	whichCaustic = static_cast<uint64_t>(uCurrentTime * 30) % 32 + 1;
	lastUpdateTime_ = now;

	accumulatedTimeInMs += elapsedTime;

	if (countForFPS++ == UPDATE_FPS_RATE) {
		AverageFPS = UPDATE_FPS_RATE * 1000 / accumulatedTimeInMs;
		countForFPS = 0;
		accumulatedTimeInMs = 0;
	}

	uWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0., 5., -75.));
	uWorld = glm::rotate(uWorld, glm::radians(glm::sin(rotation / 10.0f) * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	uWorld = glm::rotate(uWorld, glm::radians(glm::sin(rotation / 20.0f) * 30.0f), glm::vec3(1.0, 0.0, 0.0));
	uWorld = glm::scale(uWorld, glm::vec3(5.0, 5.0, 5.0));
	uWorld = glm::translate(uWorld, glm::vec3(0.0, glm::sin(rotation / 10.0) * 2.5, 0.0));

	uWorldViewProj = glm::perspective(glm::radians(ANGLE), WINDOW_WIDTH/WINDOW_HEIGHT, NEAREST, FURTHEST);
	uWorldViewProj = uWorldViewProj*uWorld;

	uWorldInvTranspose = glm::inverse(uWorldInvTranspose);
	uWorldInvTranspose = glm::transpose(uWorldInvTranspose);
}

void
AbstractJellyfish::bindUniforms()
{
	glUniformMatrix4fv(uniformMap["uWorld"].location, 1, GL_FALSE, &uWorld[0][0]);
	glUniformMatrix4fv(uniformMap["uWorldViewProj"].location, 1, GL_FALSE, &uWorldViewProj[0][0]);
	glUniformMatrix4fv(uniformMap["uWorldInvTranspose"].location, 1, GL_FALSE, &uWorldInvTranspose[0][0]);

	glUniform3fv(uniformMap["uLightPos"].location,1,&uLightPos[0]);
	glUniform1fv(uniformMap["uLightRadius"].location,1, &uLightRadius);
	glUniform4fv(uniformMap["uLightCol"].location,1,&uLightCol[0]);
	glUniform4fv(uniformMap["uAmbientCol"].location,1,&uAmbientCol[0]);
	glUniform4fv(uniformMap["uFresnelCol"].location,1,&uFresnelCol[0]);
	glUniform1fv(uniformMap["uFresnelPower"].location,1,&uFresnelPower);

	glUniform1fv(uniformMap["uCurrentTime"].location, 1, &uCurrentTime);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uSampler[0]);
	glUniform1i(uniformMap["uSampler"].location, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uSampler[whichCaustic]);
	glUniform1i(uniformMap["uSampler1"].location, 1);
}

void
AbstractJellyfish::vertexAttribPointer()
{
	for (auto u = attributeMap.begin(); u != attributeMap.end(); u++) {
		glBindBuffer(GL_ARRAY_BUFFER, u->second.buffer);
		glVertexAttribPointer(u->second.location,3,GL_FLOAT,GL_FALSE,0,(void*)0);
		//glVertexAttribPointer(attributeLocation,sizeOfAttributePerVertex,type,normalizedBoolean,stride,arrayOffset);
	}
}

//PUBLIC METHODS
void
AbstractJellyfish::initialize()
{
	createProgram();

	elementCount = (GLuint)(sizeof(index_data) / sizeof(index_data[0]));

	insertAttributes();
	getAttribLocation();
	createAndFillBuffers();

	std::vector<GLuint>  index(index_data, index_data + sizeof(index_data) / sizeof(index_data[0]));
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (index.size()) * sizeof(GL_UNSIGNED_INT), &index[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	insertUniforms();
	getUniformLocation();
	initializeTextures();
	initializeUniforms();
}

void
AbstractJellyfish::render()
{
	for (auto u = attributeMap.begin(); u != attributeMap.end(); u++) {
		glEnableVertexAttribArray(u->second.location);
	}

	glUseProgram(programID);
	updateUniforms();
	bindUniforms();
	vertexAttribPointer();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	draw();

	for (auto u = attributeMap.begin(); u != attributeMap.end(); u++) {
		glDisableVertexAttribArray(u->second.location);
	}
}

void
AbstractJellyfish::cleanup()
{
	for (auto u = attributeMap.begin(); u != attributeMap.end(); u++) {
		glDeleteBuffers(1, &u->second.buffer);
	}
	glDeleteProgram(programID);
}

