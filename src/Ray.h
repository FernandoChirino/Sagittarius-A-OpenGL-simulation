#pragma once 
#include "config.h"

struct Ray{
    // Cartesina coordinates 
    double x, y;

    // Polar coordinates
    double r, phi;
    // Polar velocities 
    double dr, dphi; 

    // Conserve quantities 
    double E, L;
    
     
    static constexpr double simulation_scale_factor = 10.0; 
    double meters_per_screen_unit; 
    float speed = 1.0;

    // Trail of points 
    std::vector<glm::vec3> trail;
    size_t maxTrailLength = 1000;

    glm::mat4 model = glm::mat4(1.0f);

    glm::vec2 dir;

    GLuint VAO, VBO;
    GLuint trailVAO, trailVBO; 

    // Constructor 
    Ray(glm::vec2 pos, glm::vec2 dir); 

    void SetupMesh();
    void Step(double dLambda, double r_s);
    //void calculateSchwarzschildGeodesic(double r_s_meter, double dt);
    void geodesicRHS(const Ray& ray, double rhs[4], double rs);
    void addState(const double a[4], const double b[4], double factor, double out[4]); 
    void rk4Step(Ray& ray, double dλ, double rs); 

    static void Draw(const std::vector<Ray>& rays, GLuint shaderProgram);
};

