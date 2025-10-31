#include "BlackHole.h"
#include "view/shader.h" 
#include "Ray.h"

const double G = 6.67430e-11; // Gravitational constant 
const double c = 299792458.0; // Speed of light in the vacuum 

// Constructor for the BlackHole class
// Initializes position, mass, and calculates the Schwarzschild radius
BlackHole::BlackHole(glm::vec3 pos, double m) : position(pos), mass(m), r_s(2.0 * G * m / (c * c))
{
    model = glm::translate(glm::mat4(1.0f), position); 
    SetupMesh(); 
}

void BlackHole::SetupMesh(){
    // Generate vertex data for a sphere representation of the black hole (UV sphere)
    std::vector<float> vertices; // interleaved: pos(3), tex(3), normal(3)
    std::vector<unsigned int> indices;

    const int sectorCount = 36; // longitudinal slices
    const int stackCount = 18;  // latitudinal stacks
    // Compute visual radius in screen/world units to match ray integration stopping radius.
    float radius = 6.0f / static_cast<float>(Ray::simulation_scale_factor);

    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * (glm::pi<float>() / stackCount); // from pi/2 to -pi/2
        float xy = radius * cosf(stackAngle); // r * cos(u)
        float z = radius * sinf(stackAngle);  // r * sin(u)

        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * (2 * glm::pi<float>() / sectorCount); // 0 to 2pi
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            // position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            // texcoord placeholder (vec3 to match shader layout)
            vertices.push_back((float)j / sectorCount);
            vertices.push_back((float)i / stackCount);
            vertices.push_back(0.0f);
            // normal (normalize position)
            glm::vec3 n = glm::normalize(glm::vec3(x, y, z));
            vertices.push_back(n.x);
            vertices.push_back(n.y);
            vertices.push_back(n.z);
        }
    }

    // indices
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);     // beginning of current stack
        int k2 = k1 + sectorCount + 1;      // beginning of next stack

        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stackCount-1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }

    indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    GLsizei stride = 9 * sizeof(float);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void BlackHole::Draw(GLuint shaderProgram){
    glUseProgram(shaderProgram);

    // Pass model matrix
    GLint ModelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Set the color uniform (e.g., red color)
    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f); // Set color to red

    glBindVertexArray(VAO);
    if (indexCount > 0) {
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}