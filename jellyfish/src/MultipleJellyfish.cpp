// Include standard headers

#include "jellyfish.h"
#include <glm/gtc/matrix_transform.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include "../util/json.hpp"

using json = nlohmann::json;

#include <fstream>
#include <string>
using namespace std;
Jellyfish::Jellyfish():offset(glm::vec3(0.,0.,0.))
{
}
Jellyfish::Jellyfish(glm::vec3 offset):offset(offset)
{
}

Jellyfish::~Jellyfish()
{
}

void
Jellyfish::createAndFillBuffers(
	std::vector<GLfloat> vertex,
	std::vector<GLfloat> normals,
	std::vector<GLfloat> colors,
	std::vector<GLfloat> texture,
	std::vector<GLuint> faces
){
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, (vertex.size())*sizeof(GL_FLOAT), &vertex[0], GL_STATIC_DRAW);

	GLuint normalsbuffer;
	glGenBuffers(1, &normalsbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
	glBufferData(GL_ARRAY_BUFFER, (normals.size())*sizeof(GL_FLOAT), &normals[0], GL_STATIC_DRAW);

	GLuint colorsbuffer;
	glGenBuffers(1, &colorsbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorsbuffer);
	glBufferData(GL_ARRAY_BUFFER, (colors.size())*sizeof(GL_FLOAT), &colors[0], GL_STATIC_DRAW);

	GLuint texturebuffer;
	glGenBuffers(1, &texturebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
	glBufferData(GL_ARRAY_BUFFER, (texture.size())*sizeof(GL_FLOAT), &texture[0], GL_STATIC_DRAW);

	GLuint facesbuffer;
	glGenBuffers(1, &facesbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (faces.size())*sizeof(GL_UNSIGNED_INT), &faces[0], GL_STATIC_DRAW);

	bufferObjects_[0] = vertexbuffer;
	bufferObjects_[1] = normalsbuffer;
	bufferObjects_[2] = colorsbuffer;
	bufferObjects_[3] = texturebuffer;
	bufferObjects_[4] = facesbuffer;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
Jellyfish::getUniformLocation()
{
	uWorldLocation = glGetUniformLocation(programID, "uWorld");
	uWorldViewProjLocation = glGetUniformLocation(programID, "uWorldViewProj");
	uWorldInvTransposeLocation = glGetUniformLocation(programID, "uWorldInvTranspose");

	uLightPosLocation = glGetUniformLocation(programID, "uLightPos");
	uLightRadiusLocation = glGetUniformLocation(programID, "uLightRadius");
	uLightColLocation = glGetUniformLocation(programID, "uLightCol");
	uAmbientColLocation = glGetUniformLocation(programID, "uAmbientCol");
	uFresnelColLocation = glGetUniformLocation(programID, "uFresnelCol");
	uFresnelPowerLocation = glGetUniformLocation(programID, "uFresnelPower");
	uCurrentTimeLocation = glGetUniformLocation(programID, "uCurrentTime");

	uSamplerLocation = glGetUniformLocation(programID, "uSampler");
	uSampler1Location = glGetUniformLocation(programID, "uSampler1");
}

void
Jellyfish::initializeUniforms()
{
	glm::mat4 uWorld = glm::mat4(1.0f);
	glm::mat4 uWorldViewProj = glm::mat4(1.0f);
	glm::mat4 uWorldInvTranspose = glm::mat4(1.0f);

	gettimeofday(&tv, NULL);

	now = static_cast<uint64_t>(tv.tv_sec) * 1000 + static_cast<uint64_t>(tv.tv_usec / 1000);
	lastUpdateTime_ = now;
	rotation = 0;
	float uCurrentTime = 0;
	int whichCaustic = 0;

	uLightPos = glm::vec3(10.0, 40.0, -60.0);
	uLightRadius = 200.0;
	uLightCol = glm::vec4(0.8, 1.3, 1.1, 1.0);
	uAmbientCol = glm::vec4(0.3, 0.2, 1.0, 1.0);
	uFresnelCol = glm::vec4(0.8, 0.7, 0.6, 1.1);
	uFresnelPower = 1.0;

	initializeTextures();
}

void
Jellyfish::initializeTextures()
{
	int length = (sizeof(images_location_data) / sizeof(images_location_data[0]));
	for (int i = 0; i < length; i++) {
		uSampler[i] = loadBMP_custom(images_location_data[i]);
	}
}

void
Jellyfish::updateUniforms()
{

	gettimeofday(&tv, NULL);
	now = static_cast<uint64_t>(tv.tv_sec) * 1000 + static_cast<uint64_t>(tv.tv_usec / 1000);
	uint64_t elapsedTime = now - lastUpdateTime_;
	rotation += (double)(2.0 * elapsedTime) / 1000.0;
	uCurrentTime = static_cast<uint64_t>(now) % 100000000 / 1000.0;
	whichCaustic = static_cast<uint64_t>(uCurrentTime * 30) % 32 + 1;
	lastUpdateTime_ = now;

	accumulatedTimeInMs += elapsedTime;

	if (countForFPS++ == 5000) {
		AverageFPS = 5000 * 1000 / accumulatedTimeInMs;
		countForFPS = 0;
		accumulatedTimeInMs = 0;
	}

	uWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0., 5., -75.));
	uWorld = glm::rotate(uWorld, glm::radians(glm::sin(rotation / 10.0f) * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	uWorld = glm::rotate(uWorld, glm::radians(glm::sin(rotation / 20.0f) * 30.0f), glm::vec3(1.0, 0.0, 0.0));
	uWorld = glm::scale(uWorld, glm::vec3(5.0, 5.0, 5.0));
	uWorld = glm::translate(uWorld, glm::vec3(0.0, glm::sin(rotation / 10.0) * 2.5, 0.0));

	uWorldViewProj = glm::perspective(glm::radians(30.0), 1920. / 1080., 20.0, 120.0);
	uWorldViewProj = uWorldViewProj*uWorld;

	uWorldInvTranspose = glm::inverse(uWorldInvTranspose);
	uWorldInvTranspose = glm::transpose(uWorldInvTranspose);
}

void
Jellyfish::bindUniforms()
{
	glUniformMatrix4fv(uWorldLocation, 1, GL_FALSE, &uWorld[0][0]);
	glUniformMatrix4fv(uWorldViewProjLocation, 1, GL_FALSE, &uWorldViewProj[0][0]);
	glUniformMatrix4fv(uWorldInvTransposeLocation, 1, GL_FALSE, &uWorldInvTranspose[0][0]);

	glUniform3fv(uLightPosLocation,1,&uLightPos[0]);
	glUniform1fv(uLightRadiusLocation,1, &uLightRadius);
	glUniform4fv(uLightColLocation,1,&uLightCol[0]);
	glUniform4fv(uAmbientColLocation,1,&uAmbientCol[0]);
	glUniform4fv(uFresnelColLocation,1,&uFresnelCol[0]);
	glUniform1fv(uFresnelPowerLocation,1,&uFresnelPower);

	glUniform1fv(uCurrentTimeLocation, 1, &uCurrentTime);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uSampler[0]);
	glUniform1i(uSamplerLocation, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uSampler[whichCaustic]);
	glUniform1i(uSampler1Location, 1);
}

void
Jellyfish::vertexAttribPointer()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[0]);
	glVertexAttribPointer(
		0,                  // attribute 0.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[1]);
	glVertexAttribPointer(
		1,                  // attribute 1.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[2]);
	glVertexAttribPointer(
		2,                  // attribute 2.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[3]);
	glVertexAttribPointer(
		3,                  // attribute 3.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjects_[4]);
}

void
Jellyfish::disableAttributes() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

//PUBLIC METHODS
void
Jellyfish::initialize()
{
	countForFPS = 0;
	accumulatedTimeInMs = 0;

	programID = LoadShaders("shaders/jellyfish.vert", "shaders/jellyfish.frag");

	std::vector<GLfloat> vertex(g_vertex_buffer_data, g_vertex_buffer_data + sizeof(g_vertex_buffer_data) / sizeof(g_vertex_buffer_data[0]));
	std::vector<GLfloat> normals(g_normals_buffer_data, g_normals_buffer_data + sizeof(g_normals_buffer_data) / sizeof(g_normals_buffer_data[0]));
	std::vector<GLfloat> colors(g_colors_buffer_data, g_colors_buffer_data + sizeof(g_colors_buffer_data) / sizeof(g_colors_buffer_data[0]));
	std::vector<GLfloat> texture(g_texture_buffer_data, g_texture_buffer_data + sizeof(g_texture_buffer_data) / sizeof(g_texture_buffer_data[0]));
	std::vector<GLuint> faces(g_faces_buffer_data, g_faces_buffer_data + sizeof(g_faces_buffer_data) / sizeof(g_faces_buffer_data[0]));

	int i = 0;
	for (std::vector<GLfloat>::iterator it = vertex.begin(); it != vertex.end(); it++) {
		(*it) += (i%3 == 0)*offset[0] + (i % 3 == 1)*offset[1] + (i % 3 == 2)*offset[2];
		i += 1;
		i %= 3;
	}

	createAndFillBuffers(vertex,normals,colors,texture,faces);
	getUniformLocation();
	initializeUniforms();
	return;
}

void
Jellyfish::draw()
{
	glUseProgram(programID);
	updateUniforms();
	bindUniforms();
	vertexAttribPointer();

	// Draw the triangles !
	glDrawElements(
			GL_TRIANGLES,      // mode
			(GLuint)(sizeof(g_faces_buffer_data) / sizeof(g_faces_buffer_data[0])),    // count
			GL_UNSIGNED_INT,   // type
			(void*)0           // element array buffer offset
	);

	disableAttributes();
}

void
Jellyfish::cleanup()
{
	glDeleteBuffers(1, &bufferObjects_[0]);
	glDeleteBuffers(1, &bufferObjects_[1]);
	glDeleteBuffers(1, &bufferObjects_[3]);
	glDeleteBuffers(1, &bufferObjects_[4]);
	glDeleteProgram(programID);

	ofstream myfile;
	myfile.open("FrameRate.txt");
	myfile << "The average Frame Rate for this session with OpenGL \nFPS: " << AverageFPS << "\n";
	myfile.close();
}


// **** JellyfishGroup **** //

JellyfishGroup::JellyfishGroup()
{
	std::ifstream filein("data/group/offset.json");//Path to Jellyfish
	std::string data;
	std::string tmp;
	if (!filein.is_open()) { std::cout << "Unable to open file \n"; }
	while (filein >> tmp) {
		data += tmp;
	}
	auto j3 = json::parse(data);
	for (json::iterator it = j3.begin(); it != j3.end(); ++it) {
		vec3 offset((*it)[0], (*it)[1], (*it)[2]);
		group.push_back(new Jellyfish(offset));
	}
}

JellyfishGroup::~JellyfishGroup()
{
}

void
JellyfishGroup::initialize() {
	for (std::vector<Jellyfish* >::iterator it = group.begin(); it != group.end(); it++) {
		(*it)->initialize();
	}
}
void
JellyfishGroup::draw() {
	for (std::vector<Jellyfish* >::iterator it = group.begin(); it != group.end(); it++) {
		(*it)->draw();
	}
}
void
JellyfishGroup::cleanup() {
	for (std::vector<Jellyfish* >::iterator it = group.begin(); it != group.end(); it++) {
		(*it)->cleanup();
	}
}

JellyfishInstanced::JellyfishInstanced()
{
}

JellyfishInstanced::~JellyfishInstanced()
{
}

void
JellyfishInstanced::createAndFillBuffers(
	std::vector<GLfloat> vertex,
	std::vector<GLfloat> normals,
	std::vector<GLfloat> colors,
	std::vector<GLfloat> texture,
	std::vector<GLfloat> offset,
	std::vector<GLuint> faces
) {
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, (vertex.size()) * sizeof(GL_FLOAT), &vertex[0], GL_STATIC_DRAW);

	GLuint normalsbuffer;
	glGenBuffers(1, &normalsbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
	glBufferData(GL_ARRAY_BUFFER, (normals.size()) * sizeof(GL_FLOAT), &normals[0], GL_STATIC_DRAW);

	GLuint colorsbuffer;
	glGenBuffers(1, &colorsbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorsbuffer);
	glBufferData(GL_ARRAY_BUFFER, (colors.size()) * sizeof(GL_FLOAT), &colors[0], GL_STATIC_DRAW);

	GLuint texturebuffer;
	glGenBuffers(1, &texturebuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texturebuffer);
	glBufferData(GL_ARRAY_BUFFER, (texture.size()) * sizeof(GL_FLOAT), &texture[0], GL_STATIC_DRAW);

	GLuint offsetbuffer;
	glGenBuffers(1, &offsetbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, offsetbuffer);
	glBufferData(GL_ARRAY_BUFFER, (offset.size()) * sizeof(GL_FLOAT), &offset[0], GL_STATIC_DRAW);

	GLuint facesbuffer;
	glGenBuffers(1, &facesbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (faces.size()) * sizeof(GL_UNSIGNED_INT), &faces[0], GL_STATIC_DRAW);

	bufferObjects_[0] = vertexbuffer;
	bufferObjects_[1] = normalsbuffer;
	bufferObjects_[2] = colorsbuffer;
	bufferObjects_[3] = texturebuffer;
	bufferObjects_[4] = offsetbuffer;

	bufferObjects_[5] = facesbuffer;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void
JellyfishInstanced::parseOffset(std::vector<GLfloat>& offset)
{
	std::ifstream filein("data/group/offset.json");
	std::string data;
	std::string tmp;
	if (!filein.is_open()) { std::cout << "Unable to open file \n"; }
	while (filein >> tmp) {
		data += tmp;
	}
	auto j3 = json::parse(data);
	for (json::iterator it = j3.begin(); it != j3.end(); ++it) {
		offset.push_back((*it)[0]);
		offset.push_back((*it)[1]);
		offset.push_back((*it)[2]);
	}
	numberOfJellyfish = j3.size();
}

void
JellyfishInstanced::getUniformLocation()
{
	uWorldLocation = glGetUniformLocation(programID, "uWorld");
	uWorldViewProjLocation = glGetUniformLocation(programID, "uWorldViewProj");
	uWorldInvTransposeLocation = glGetUniformLocation(programID, "uWorldInvTranspose");

	uLightPosLocation = glGetUniformLocation(programID, "uLightPos");
	uLightRadiusLocation = glGetUniformLocation(programID, "uLightRadius");
	uLightColLocation = glGetUniformLocation(programID, "uLightCol");
	uAmbientColLocation = glGetUniformLocation(programID, "uAmbientCol");
	uFresnelColLocation = glGetUniformLocation(programID, "uFresnelCol");
	uFresnelPowerLocation = glGetUniformLocation(programID, "uFresnelPower");
	uCurrentTimeLocation = glGetUniformLocation(programID, "uCurrentTime");

	uSamplerLocation = glGetUniformLocation(programID, "uSampler");
	uSampler1Location = glGetUniformLocation(programID, "uSampler1");
}

void
JellyfishInstanced::initializeUniforms()
{
	glm::mat4 uWorld = glm::mat4(1.0f);
	glm::mat4 uWorldViewProj = glm::mat4(1.0f);
	glm::mat4 uWorldInvTranspose = glm::mat4(1.0f);

	gettimeofday(&tv, NULL);

	now = static_cast<uint64_t>(tv.tv_sec) * 1000 + static_cast<uint64_t>(tv.tv_usec / 1000);
	lastUpdateTime_ = now;
	rotation = 0;
	float uCurrentTime = 0;
	int whichCaustic = 0;

	uLightPos = glm::vec3(10.0, 40.0, -60.0);
	uLightRadius = 200.0;
	uLightCol = glm::vec4(0.8, 1.3, 1.1, 1.0);
	uAmbientCol = glm::vec4(0.3, 0.2, 1.0, 1.0);
	uFresnelCol = glm::vec4(0.8, 0.7, 0.6, 1.1);
	uFresnelPower = 1.0;

	initializeTextures();
}

void
JellyfishInstanced::initializeTextures()
{
	int length = (sizeof(images_location_data) / sizeof(images_location_data[0]));
	for (int i = 0; i < length; i++) {
		uSampler[i] = loadBMP_custom(images_location_data[i]);
	}
}

void
JellyfishInstanced::updateUniforms()
{

	gettimeofday(&tv, NULL);
	now = static_cast<uint64_t>(tv.tv_sec) * 1000 + static_cast<uint64_t>(tv.tv_usec / 1000);
	uint64_t elapsedTime = now - lastUpdateTime_;
	rotation += (double)(2.0 * elapsedTime) / 1000.0;
	uCurrentTime = static_cast<uint64_t>(now) % 100000000 / 1000.0;
	whichCaustic = static_cast<uint64_t>(uCurrentTime * 30) % 32 + 1;
	lastUpdateTime_ = now;

	accumulatedTimeInMs += elapsedTime;

	if (countForFPS++ == 5000) {
		AverageFPS = 5000 * 1000 / accumulatedTimeInMs;
		countForFPS = 0;
		accumulatedTimeInMs = 0;
	}

	uWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0., 5., -75.));
	uWorld = glm::rotate(uWorld, glm::radians(glm::sin(rotation / 10.0f) * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	uWorld = glm::rotate(uWorld, glm::radians(glm::sin(rotation / 20.0f) * 30.0f), glm::vec3(1.0, 0.0, 0.0));
	uWorld = glm::scale(uWorld, glm::vec3(5.0, 5.0, 5.0));
	uWorld = glm::translate(uWorld, glm::vec3(0.0, glm::sin(rotation / 10.0) * 2.5, 0.0));

	uWorldViewProj = glm::perspective(glm::radians(30.0), 1920. / 1080., 20.0, 120.0);
	uWorldViewProj = uWorldViewProj*uWorld;

	uWorldInvTranspose = glm::inverse(uWorldInvTranspose);
	uWorldInvTranspose = glm::transpose(uWorldInvTranspose);
}

void
JellyfishInstanced::bindUniforms()
{
	glUniformMatrix4fv(uWorldLocation, 1, GL_FALSE, &uWorld[0][0]);
	glUniformMatrix4fv(uWorldViewProjLocation, 1, GL_FALSE, &uWorldViewProj[0][0]);
	glUniformMatrix4fv(uWorldInvTransposeLocation, 1, GL_FALSE, &uWorldInvTranspose[0][0]);

	glUniform3fv(uLightPosLocation, 1, &uLightPos[0]);
	glUniform1fv(uLightRadiusLocation, 1, &uLightRadius);
	glUniform4fv(uLightColLocation, 1, &uLightCol[0]);
	glUniform4fv(uAmbientColLocation, 1, &uAmbientCol[0]);
	glUniform4fv(uFresnelColLocation, 1, &uFresnelCol[0]);
	glUniform1fv(uFresnelPowerLocation, 1, &uFresnelPower);

	glUniform1fv(uCurrentTimeLocation, 1, &uCurrentTime);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uSampler[0]);
	glUniform1i(uSamplerLocation, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, uSampler[whichCaustic]);
	glUniform1i(uSampler1Location, 1);
}

void
JellyfishInstanced::vertexAttribPointer()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[0]);
	glVertexAttribPointer(
		0,                  // attribute 0.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[1]);
	glVertexAttribPointer(
		1,                  // attribute 1.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[2]);
	glVertexAttribPointer(
		2,                  // attribute 2.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[3]);
	glVertexAttribPointer(
		3,                  // attribute 3.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObjects_[4]);
	glVertexAttribPointer(
		4,                  // attribute 4.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);
	glVertexAttribDivisor(4, 1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjects_[5]);
}

void
JellyfishInstanced::disableAttributes() {
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);
}

//PUBLIC METHODS
void
JellyfishInstanced::initialize()
{
	countForFPS = 0;
	accumulatedTimeInMs = 0;

	programID = LoadShaders("shaders/jellyfishInstanced.vert", "shaders/jellyfish.frag");

	std::vector<GLfloat> vertex(g_vertex_buffer_data, g_vertex_buffer_data + sizeof(g_vertex_buffer_data) / sizeof(g_vertex_buffer_data[0]));
	std::vector<GLfloat> normals(g_normals_buffer_data, g_normals_buffer_data + sizeof(g_normals_buffer_data) / sizeof(g_normals_buffer_data[0]));
	std::vector<GLfloat> colors(g_colors_buffer_data, g_colors_buffer_data + sizeof(g_colors_buffer_data) / sizeof(g_colors_buffer_data[0]));
	std::vector<GLfloat> texture(g_texture_buffer_data, g_texture_buffer_data + sizeof(g_texture_buffer_data) / sizeof(g_texture_buffer_data[0]));
	std::vector<GLfloat> offset;
	std::vector<GLuint> faces(g_faces_buffer_data, g_faces_buffer_data + sizeof(g_faces_buffer_data) / sizeof(g_faces_buffer_data[0]));
	parseOffset(offset);

	createAndFillBuffers(vertex, normals, colors, texture, offset, faces);
	getUniformLocation();
	initializeUniforms();
	return;
}

void
JellyfishInstanced::draw()
{
	glUseProgram(programID);
	updateUniforms();
	bindUniforms();
	vertexAttribPointer();

	glDrawElementsInstanced(
		GL_TRIANGLES,      // mode
		(GLuint)(sizeof(g_faces_buffer_data) / sizeof(g_faces_buffer_data[0])),    // count
		GL_UNSIGNED_INT,   // type
		(void*)0,           // element array buffer offset
		numberOfJellyfish
	);

	disableAttributes();
}

void
JellyfishInstanced::cleanup()
{
	glDeleteBuffers(1, &bufferObjects_[0]);
	glDeleteBuffers(1, &bufferObjects_[1]);
	glDeleteBuffers(1, &bufferObjects_[2]);
	glDeleteBuffers(1, &bufferObjects_[3]);
	glDeleteBuffers(1, &bufferObjects_[4]);
	glDeleteBuffers(1, &bufferObjects_[5]);
	glDeleteProgram(programID);

	ofstream myfile;
	myfile.open("FrameRateInstanced.txt");
	myfile << "The average Frame Rate for this session FrameRateInstanced with OpenGL \nFPS: " << AverageFPS << "\n";
	myfile.close();
}
