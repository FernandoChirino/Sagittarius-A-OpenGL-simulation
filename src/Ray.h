#pragma once 
#include "config.h"

struct Ray{
    // Cartesina coordinates 
    double x, y;

    // Polar coordinates
    double r, phi;
    double dr, dphi;

    glm::vec2 dir;

    // Trail of points 
    std::vector<glm::vec2> trail;

    // Conserve quantities 
    double E, L;

    const float speed = 0.01f; 

    glm::mat4 model = glm::mat4(1.0f);

    GLuint VAO, VBO;
    GLuint trailVAO, trailVBO; 

    // Constructor 
    Ray(glm::vec2 pos, glm::vec2 dir, double r_s); 

    void SetupMesh();
    void Step(double dLambda, double r_s);
    static void Draw(const std::vector<Ray>& rays, GLuint shaderProgram);
};

void geodesicRHS(const Ray& ray, double rhs[4], double rs);
void addState(const double a[4], const double b[4], double factor, double out[4]);
void rk4Step(Ray& ray, double dLambda, double rs); 
