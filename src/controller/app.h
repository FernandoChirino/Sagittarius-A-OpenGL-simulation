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
    std::vector<Ray> InitializeRays(int numRays);
    
    GLFWwindow* window;
    unsigned int shader;

    static constexpr double simulation_scale_factor = 10.0; // How many Schwarzschild radii fit across screen

    //Timing
    double lastTime, currentTime;
	int numFrames;
	float frameTime;
};