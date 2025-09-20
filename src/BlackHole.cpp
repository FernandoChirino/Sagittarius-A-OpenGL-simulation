#include "BlackHole.h"
#include "view/shader.h" 

// Define constants for gravitational constant and speed of light
const double G = 6.67430e-11; // Gravitational constant 
const double c = 299792458.0; // Speed of light in the vacuum 

// Constructor for the BlackHole class
// Initializes position, mass, and calculates the Schwarzschild radius
BlackHole::BlackHole(glm::vec3 pos, double m) : position(pos), mass(m), r_s(2.0 * G * m / (c * c))
{
    SetupMesh(); // Set up the mesh for rendering the black hole
}

// Function to set up the mesh for rendering the black hole
void BlackHole::SetupMesh(){
    // Generate vertex data for a circular representation of the black hole
    std::vector<float> vertices;

    int numSegments = 100; // Number of segments for the circle
    float radius = (float)r_s * 1e-10f; // Scale down the radius for visualization

    for (int i = 0; i <= numSegments; i++){
        float angle = 2.0f * glm::pi<float>() * i / numSegments;
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        vertices.push_back(x);
        vertices.push_back(y);
        vertices.push_back(0.0f); // Add z-coordinate
    }

    // Generate and bind vertex array and buffer objects
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // Define vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE , 3 * sizeof(float), nullptr); // Update to vec3
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

// Function to draw the black hole using the specified shader program
void BlackHole::Draw(GLuint shaderProgram){
    glUseProgram(shaderProgram);

    // Set the color uniform (e.g., red color)
    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f); // Set color to red

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 101); // Draw the circular mesh
    glBindVertexArray(0);
}