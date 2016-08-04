// Include standard headers
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;


#include <common/shader.hpp>
#include <common/texture.hpp>
#include "data/conf.h"

#include "src/AbstractJellyfish.h"
#include "src/SingleJellyfish.h"
#include "src/InstancedJellyfish.h"
#include "src/gradient.h"

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
    if(FULL_SCREEN){
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Jellyfish Benchmark",  glfwGetPrimaryMonitor(), NULL);
    }else{
        window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Jellyfish Benchmark", NULL, NULL);
    }
	

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	cout<< "This is width :"  << width<<"\n";
	cout<< "This is height :" << height<<"\n";

	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.1f, 0.1f, 0.5f, 0.0f);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND); Drastically increase/decrease performances
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFrontFace(GL_CW);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	SingleJellyfish* jellyfish = new SingleJellyfish();
	Gradient* gradient = new Gradient();

	jellyfish->initialize();
	gradient->initialize();

	do{

		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
		gradient->draw();
		jellyfish->render();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );
	gradient->cleanup();
	jellyfish->cleanup();

	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}
