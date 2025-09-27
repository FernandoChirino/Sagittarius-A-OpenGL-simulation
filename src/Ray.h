#pragma once 
#include "config.h"

struct Ray{
    // Cartesina coordinates 
    double x, y;

    // Polar coordinates
    double r, phi;

    // Conserve quantities 
    double E, L;
    
    const double speed = 0.5; 
    static constexpr double simulation_scale_factor = 10.0; 
    double meters_per_screen_unit; 

    // Trail of points 
    std::vector<glm::vec3> trail;
    size_t maxTrailLength = 1000;

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec2 dir;

    GLuint VAO, VBO;
    GLuint trailVAO, trailVBO; 

    // Constructor 
    Ray(glm::vec2 pos, glm::vec2 dir, double r_s); 

    void SetupMesh();
    void Step(double dLambda, double r_s);
    void calculateSchwarzschildGeodesic(double r_s_meter, double dt);

    static void Draw(const std::vector<Ray>& rays, GLuint shaderProgram);
};

