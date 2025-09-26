#include "Ray.h"
#include "view/shader.h" 
#include "config.h"
#include <cmath>

Ray::Ray(glm::vec2 pos, glm::vec2 dir, double r_s) : x(pos.x), y(pos.y), dir(dir){ 
    // Get polar coordinates
    r = sqrt(x*x + y*y);
    phi = atan2(y,x);

    // Seed velocities 
    dr = dir.x * cos(phi) + dir.y * sin(phi);
    dphi = (-dir.x * sin(phi) + dir.y * cos(phi)) / r; 

    // Start trail
    trail.push_back({x,y}); 

    model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f));
    SetupMesh();
}

void Ray::SetupMesh(){
    // --- Setup point ---
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Initially with a single point 
    glm::vec2 point = glm::vec2(0.0f, 0.0f);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2), &point, GL_DYNAMIC_DRAW);

    // Vertex attribute 0: position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    // --- Setup trail ---
    glGenVertexArrays(1, &trailVAO);
    glGenBuffers(1, &trailVBO);

    glBindVertexArray(trailVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trailVBO);

    // empty buffer for now, will update every frame
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Vertex attribute 0: position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Ray::Step(double dLambda, double r_s){
    x += dir.x * speed;
    y += dir.y * speed;

    trail.push_back(glm::vec2(x, y)); // Update trail after position update

    r = sqrt(x*x + y*y); 

    //if (r <= r_s) return; // Stop if inside the event horizon

    model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}

void Ray::Draw(const std::vector<Ray>& rays, GLuint shaderProgram){
    glUseProgram(shaderProgram);

    // Turn on blending for the trails
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glLineWidth(1.0f);

    for (auto& ray: rays){
        // ---- Draw Ray trail ----
        if (!ray.trail.empty()){
            glBindBuffer(GL_ARRAY_BUFFER, ray.trailVBO);
            glBufferData(GL_ARRAY_BUFFER,
                        ray.trail.size() * sizeof(glm::vec2),
                        ray.trail.data(),
                        GL_DYNAMIC_DRAW); 
            
            GLint ModelLoc = glGetUniformLocation(shaderProgram, "model");
            glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Use identity matrix for trail
             
            GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
            glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);

            glBindVertexArray(ray.trailVAO);
            glDrawArrays(GL_LINE_STRIP, 0, ray.trail.size());
            glBindVertexArray(0);
        }

        // ---- Draw the Ray head ----
        GLint ModelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(ModelLoc, 1, GL_FALSE, glm::value_ptr(ray.model));

        GLint colorLocation = glGetUniformLocation(shaderProgram, "color");
        glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f); 

        glBindVertexArray(ray.VAO);
        glPointSize(8.0f);
        glDrawArrays(GL_POINTS, 0, 1); 
        glBindVertexArray(0);
    }

    glDisable(GL_BLEND);

}

