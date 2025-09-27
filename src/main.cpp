// Force program to run on dedicated GPU. Remove if you want to run on your CPU's integrated graphics. 
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

#include "config.h"
#include "controller/app.h"
#include "BlackHole.h"

int main() {
	srand(static_cast<unsigned int>(time(nullptr)));

	// Create an instance of the App class to manage the application
	App* app = new App();

	// Create a black hole object with a specific position and mass
	BlackHole Sagitarius(glm::vec3(0.0f, 0.0f, 0.0f), 8.54e36);

	// Set up OpenGL context and initialize the application
	app->set_up_opengl();

	// Run the application with the black hole object
	app->run(Sagitarius);

	// Clean up and release resources
	delete app;
	return 0;
}