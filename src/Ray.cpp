#include "Ray.h"
#include "view/shader.h" 
#include "config.h"
#include <cmath>

Ray::Ray(glm::vec2 pos, glm::vec2 dir) : x(pos.x), y(pos.y), dir(dir){ 
    // Get polar coordinates
    r = sqrt(x*x + y*y);
    phi = atan2(y,x);

    // Polar velocities 
    dr = dir.x * cos(phi) + dir.y * sin(phi);
    dphi = (-dir.x * sin(phi) + dir.y * cos(phi)) / r; 

    E = 1.0; 

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
    // Convert Schwarzschild radius to screen coordinates
    meters_per_screen_unit = (r_s_meters * simulation_scale_factor) / 6;
    double r_s_screen = r_s_meters / meters_per_screen_unit;

    r = hypot(x, y);
    if (r <= r_s_screen * 1.05) return; // Stop if inside the event horizon

    //calculateSchwarzschildGeodesic;
    rk4Step(*this, dLambda, r_s_screen); 

    
    x = r * cos(phi);
    y = r * sin(phi);

    dir.x = dr * cos(phi) - r * sin(phi) * dphi;
    dir.y = dr * sin(phi) + r * cos(phi) * dphi;

    // normalize direction 
    if (glm::length(dir) > 0) {
        dir = glm::normalize(dir) * speed;
    }

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


void Ray::geodesicRHS(const Ray& ray, double rhs[4], double rs){
    double r    = ray.r;
    double dr   = ray.dr;
    double dphi = ray.dphi;
    double E    = ray.E;

    // Prevents calculatios too close to the event horizon
    if (r <= rs * 1.01){
        rhs[0] = 0; // dr/dλ = 0
        rhs[1] = 0; // dφ/dλ = 0
        rhs[2] = 0; // d²r/dλ² = 0
        rhs[3] = 0; // d²φ/dλ² = 0
        return;
    }

    double f = 1.0 - rs/r;

    // Avoid numerical issues when f is very small
    if (f < 1e-10) {
        rhs[0] = 0;
        rhs[1] = 0;
        rhs[2] = 0;
        rhs[3] = 0;
        return;
    }

    // dr/dλ = dr
    rhs[0] = dr;
    // dφ/dλ = dphi
    rhs[1] = dphi;

    // d²r/dλ² from Schwarzschild null geodesic:
    double dt_dλ = E / f;
    rhs[2] = 
        - (rs/(2*r*r)) * f * (dt_dλ*dt_dλ)
        + (rs/(2*r*r*f)) * (dr*dr)
        + (r - rs) * (dphi*dphi);

    // d²φ/dλ² = -2*(dr * dphi) / r
    rhs[3] = -2.0 * dr * dphi / r;
}

void Ray::addState(const double a[4], const double b[4], double factor, double out[4]) {
    for (int i = 0; i < 4; i++)
        out[i] = a[i] + b[i] * factor;
}

void Ray::rk4Step(Ray& ray, double dλ, double rs) {
    double y0[4] = { ray.r, ray.phi, ray.dr, ray.dphi };
    double k1[4], k2[4], k3[4], k4[4], temp[4];

    geodesicRHS(ray, k1, rs);
    addState(y0, k1, dλ/2.0, temp);
    Ray r2 = ray; r2.r=temp[0]; r2.phi=temp[1]; r2.dr=temp[2]; r2.dphi=temp[3];
    geodesicRHS(r2, k2, rs);

    addState(y0, k2, dλ/2.0, temp);
    Ray r3 = ray; r3.r=temp[0]; r3.phi=temp[1]; r3.dr=temp[2]; r3.dphi=temp[3];
    geodesicRHS(r3, k3, rs);

    addState(y0, k3, dλ, temp);
    Ray r4 = ray; r4.r=temp[0]; r4.phi=temp[1]; r4.dr=temp[2]; r4.dphi=temp[3];
    geodesicRHS(r4, k4, rs);

    ray.r    += (dλ/6.0)*(k1[0] + 2*k2[0] + 2*k3[0] + k4[0]);
    ray.phi  += (dλ/6.0)*(k1[1] + 2*k2[1] + 2*k3[1] + k4[1]);
    ray.dr   += (dλ/6.0)*(k1[2] + 2*k2[2] + 2*k3[2] + k4[2]);
    ray.dphi += (dλ/6.0)*(k1[3] + 2*k2[3] + 2*k3[3] + k4[3]);
}
