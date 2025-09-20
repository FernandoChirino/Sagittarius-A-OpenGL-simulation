#pragma once 
#include "config.h"

// Class representing a black hole in the simulation
struct BlackHole{
    glm::vec3 position; // Position of the black hole in 3D space
    double mass;        // Mass of the black hole
    double radius;      // Radius of the black hole (not used in this implementation)
    double r_s;         // Schwarzschild radius (event horizon radius)

    GLuint VAO, VBO;    // OpenGL Vertex Array Object and Vertex Buffer Object

    // Constructor to initialize the black hole with position and mass
    BlackHole(glm::vec3 pos, double m);

    // Function to set up the mesh for rendering the black hole
    void SetupMesh();

    // Function to draw the black hole using a shader program
    void Draw(GLuint shaderProgram);
};