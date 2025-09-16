#pragma once
#include "../config.h"

class App {
public:
    App();
    ~App();
    void run();
    void set_up_opengl();
    void make_systems();
    
private:
    void set_up_glfw();
    void handle_frame_timing();

    GLFWwindow* window;

    unsigned int shader;

    //Timing
    double lastTime, currentTime;
	int numFrames;
	float frameTime;
};