#ifndef INSTANCED_JELLYFISH_
#define INSTANCED_JELLYFISH_

#include "AbstractJellyfish.h"

class InstancedJellyfish : public AbstractJellyfish
{
private:
	void createProgram();
	void insertAttributes();
	void draw();
	void vertexAttribPointer();

	int numberOfJellyfish;
public:
	InstancedJellyfish();
	~InstancedJellyfish();
	void cleanup();
};

#endif
