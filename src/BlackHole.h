#pragma once 
#include "config.h"
struct BlackHole{
    glm::vec3 position; 
    double mass;        
    double radius;     
    double r_s;         // Schwarzschild radius (event horizon radius)

    GLuint VAO, VBO;    

    // Constructor to initialize the black hole with position and mass
    BlackHole(glm::vec3 pos, double m);

    // Set up the mesh for rendering the black hole
    void SetupMesh();
    void Draw(GLuint shaderProgram);
};