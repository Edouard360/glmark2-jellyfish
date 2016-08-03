#include "SingleJellyfish.h"

SingleJellyfish::SingleJellyfish()
{
}

SingleJellyfish::~SingleJellyfish()
{
}

void
SingleJellyfish::createProgram() {
	programID = LoadShaders("shaders/jellyfish.vert", "shaders/jellyfish.frag");
}
void
SingleJellyfish::draw() {
	glDrawElements(GL_TRIANGLES, elementCount, GL_UNSIGNED_INT, (void*)0);
}

void
SingleJellyfish::cleanup()
{
	AbstractJellyfish::cleanup();
	ofstream myfile;
	myfile.open("FrameRate.txt");
	myfile << "The average Frame Rate for this session with OpenGL \nFPS: " << AverageFPS << "\n";
	myfile.close();
}