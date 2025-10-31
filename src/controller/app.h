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

    // Simple orbiting camera
    float camRadius = 5.0f;
    float camAzimuth = 0.0f;   // horizontal angle (radians)
    float camElevation = 0.0f; // vertical angle (radians)
    float camSensitivity = 0.005f;
    float camZoomSpeed = 0.5f;

    // Mouse state
    bool rotating = false;
    double lastMouseX = 0.0, lastMouseY = 0.0;

    // Update the view uniform from camera parameters
    void updateViewUniform();

    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
};