#ifndef SINGLE_JELLYFISH_
#define SINGLE_JELLYFISH_

#include "AbstractJellyfish.h";

class SingleJellyfish :public AbstractJellyfish
{
private:
	void createProgram();
	void draw();
public:
	SingleJellyfish();
    ~SingleJellyfish();
	void cleanup();
};

#endif
