#pragma once 
#include "config.h"
struct BlackHole{
    glm::vec3 position; 
    double mass;        
    double radius;     
    double r_s;         // Schwarzschild radius (event horizon radius)
    glm::mat4 model = glm::mat4(1.0f);

    GLuint VAO, VBO;    
    GLuint EBO;
    GLsizei indexCount = 0;

    // Constructor to initialize the black hole with position and mass
    BlackHole(glm::vec3 pos, double m);

    // Set up the mesh for rendering the black hole
    void SetupMesh();
    void Draw(GLuint shaderProgram);
};