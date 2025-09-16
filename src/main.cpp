extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

#include "config.h"
#include "controller/app.h"

int main() {

	App* app = new App();

	app->set_up_opengl();

	app->run();

	delete app;
	return 0;
}