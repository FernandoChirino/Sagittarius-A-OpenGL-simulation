#pragma once
#include "../config.h"
#include "../BlackHole.h"
#include "../Ray.h"

class App {
public:
    App();
    ~App();
    void run(BlackHole& blackhole);
    void set_up_opengl();
    void make_systems();
    
private:
    void set_up_glfw();
    void handle_frame_timing();
    std::vector<Ray> InitializeRays(int numRays, double r_s);

    GLFWwindow* window;

    unsigned int shader;

    //Timing
    double lastTime, currentTime;
	int numFrames;
	float frameTime;
};