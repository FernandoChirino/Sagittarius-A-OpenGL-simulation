#include "Ray.h"
#include "view/shader.h" 
#include "config.h"
#include <cmath>

Ray::Ray(glm::vec2 pos, glm::vec2 dir, double r_s) : x(pos.x), y(pos.y), dir(dir){ 
    // Get polar coordinates
    r = sqrt(x*x + y*y);
    phi = atan2(y,x);

    // Seed velocities 
    // dr = dir.x * cos(phi) + dir.y * sin(phi);
    // dphi = (-dir.x * sin(phi) + dir.y * cos(phi)) / r; 

    // Start trail
    trail.push_back({x,y, 1.0f}); 

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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // Vertex atttribute 3: alpha
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

void Ray::Step(double dLambda, double r_s_meters){
    r = hypot(x, y);
    
    // Convert Schwarzschild radius to screen coordinates
    meters_per_screen_unit = (r_s_meters * simulation_scale_factor) / 6.0;
    double r_s_screen = r_s_meters / meters_per_screen_unit;

    if (r <= r_s_screen) return; // Stop if inside the event horizon

    calculateSchwarzschildGeodesic(r_s_meters, dLambda);

    x += dir.x * speed * dLambda;
    y += dir.y * speed * dLambda;

    trail.push_back(glm::vec3(x, y, 1.0f)); // Update trail after position update

    // Limit the trail size 
    if (trail.size() > maxTrailLength) {
        trail.erase(trail.begin());
    }

    // Update alpha values
    if (trail.size() > 1){
        for (size_t i = 0; i < trail.size(); i ++){
            float normalizedAge = (float)i / (float)(trail.size() - 1);
            trail[i].z = normalizedAge;
        }
    }

    
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
                        ray.trail.size() * sizeof(glm::vec3),
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
        glPointSize(6.0f);
        glDrawArrays(GL_POINTS, 0, 1); 
        glBindVertexArray(0);
    }

    glDisable(GL_BLEND);

}

void Ray::calculateSchwarzschildGeodesic(double r_s_meters, double dt){
    // Current pos in meters 
    double x_m = x * meters_per_screen_unit;
    double y_m = y * meters_per_screen_unit;
    double r_m = hypot(x_m, y_m);

    phi = atan2(y_m, x_m);
    glm::vec2 old_dir = dir;

    // Velocities in physical units 
    double vx_m = dir.x * speed * meters_per_screen_unit;
    double vy_m = dir.y * speed * meters_per_screen_unit;

    // Radial and angular velocities
    double vr = (x_m * vx_m + y_m * vy_m) / r_m;
    double vphi = (x_m * vy_m - y_m * vx_m) / (r_m * r_m);

    // Schwarzschild metric components
    double f = 1.0 - r_s_meters / r_m;
    
    // Geodesic equations (simplified for photons/massless particles)
    // d²r/dτ² = -GM/r² + L²/r³ - 3GM*L²/(2r⁴c²)
    double L = r_m * r_m * vphi; // Angular momentum 
    constexpr double GM = 1.327e20;

    // Radial acceleration
    double ar = -GM / (r_m * r_m) + L * L / (r_m * r_m * r_m) 
                - 3.0 * GM * L * L / (2.0 * pow(r_m, 4) * 299792458.0 * 299792458.0);

    // Angular acceleration
    double aphi = -2.0 * vr * vphi / r_m;

    vr += ar * dt;
    vphi += aphi * dt;

    // Convert back to Cartesian velocities
    double cos_phi = cos(phi);
    double sin_phi = sin(phi);

    vx_m = vr * cos_phi - r_m * vphi * sin_phi;
    vy_m = vr * sin_phi + r_m * vphi * cos_phi;
    
    // Convert back to screen coordinates and update direction
    dir.x = vx_m / (speed * meters_per_screen_unit);
    dir.y = vy_m / (speed * meters_per_screen_unit);
    
    dir = glm::normalize(dir);

    // DEBUG
    static int debug_counter = 0;
    if (debug_counter % 60 == 0) {
        std::cout << "Geodesic: r=" << r_m/r_s_meters << "Rs, "
                  << "old_dir=(" << old_dir.x << "," << old_dir.y << "), "
                  << "new_dir=(" << dir.x << "," << dir.y << "), "
                  << "ar=" << ar << ", aphi=" << aphi << std::endl;
    }
    debug_counter++;
}