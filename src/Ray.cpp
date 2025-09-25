#include "Ray.h"
#include "view/shader.h" 
#include "config.h"
#include <cmath>

Ray::Ray(glm::vec2 pos, glm::vec2 dir, double r_s) : x(pos.x), y(pos.y){
    model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    SetupMesh();
}

void Ray::SetupMesh(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Initially with a single point 
    float vertices[2] = {float(x), float(y)};
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    // Vertex attribute 0: position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Ray::Step(double dLambda, double r_s){
    double lambda = dLambda;
    if (r <= r_s) return; // Stop if inside the event horizon
}

void Ray::Draw(const std::vector<Ray>& rays, GLuint shaderProgram){
    glUseProgram(shaderProgram);

    for (auto& ray: rays){
        // Pass model matrix
        GLint ModelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(ray.model));

        GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f); 

        glBindVertexArray(ray.VAO);
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 0, 1); 
        glBindVertexArray(0);
    }

}

