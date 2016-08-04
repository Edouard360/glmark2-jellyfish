#include "InstancedJellyfish.h"

#include <stdlib.h> 
#include "../util/json.hpp"
#include "../data/conf.h"
using json = nlohmann::json;

InstancedJellyfish::InstancedJellyfish()
{
}

InstancedJellyfish::~InstancedJellyfish()
{
}

void
InstancedJellyfish::createProgram() {
	programID = LoadShaders("shaders/jellyfishInstanced.vert", "shaders/jellyfish.frag");
}

void
InstancedJellyfish::insertAttributes() {
	AbstractJellyfish::insertAttributes();

	//srand()

	std::vector<GLfloat> offset;
	for (int i =0;i<MAX_NUMBER;i++) {
		offset.push_back(rand()%10 - 5);
		offset.push_back(rand()%10 - 5);
		offset.push_back(rand()%10 - 5);
	}

	// ** TO GENERATE OFFSET FROM JSON FILE
	// std::ifstream filein("data/group/offset.json");
	// std::string data;
	// std::string tmp;
	// if (!filein.is_open()) { std::cout << "Unable to open file \n"; }
	// while (filein >> tmp) {
	// 	data += tmp;
	// }
	// auto j3 = json::parse(data);
	// for (json::iterator it = j3.begin(); it != j3.end(); ++it) {
	// 	offset.push_back((*it)[0]);
	// 	offset.push_back((*it)[1]);
	// 	offset.push_back((*it)[2]);
	// }
	// numberOfJellyfish = j3.size();
	numberOfJellyfish = MAX_NUMBER;

	attributeMap.insert(pair<string, attribute>("offset", { 0, 0 ,offset }));
}

void
InstancedJellyfish::vertexAttribPointer()
{
	AbstractJellyfish::vertexAttribPointer();
	glVertexAttribDivisor(attributeMap["offset"].location, 1);
}

void
InstancedJellyfish::draw()
{
	glDrawElementsInstanced(GL_TRIANGLES,elementCount,GL_UNSIGNED_INT,(void*)0,numberOfJellyfish);
}

//PUBLIC METHOD

void
InstancedJellyfish::cleanup()
{
	AbstractJellyfish::cleanup();
	ofstream myfile;
	myfile.open("FrameRateInstanced.txt");
	myfile << "The average Frame Rate for this session FrameRateInstanced with OpenGL \nFPS: " << AverageFPS << "\n";
	myfile.close();
}
