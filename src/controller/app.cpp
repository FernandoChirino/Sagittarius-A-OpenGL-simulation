#include "app.h"
#include "../view/shader.h" 
#include "../BlackHole.h"
#include "../Ray.h"

// Constructor for the App class
// Sets up GLFW for window and context management
App::App() {
    set_up_glfw();
}

// Destructor for the App class
// Cleans up resources and terminates GLFW
App::~App() {
    glDeleteProgram(shader); // Delete the shader program
    glfwTerminate(); // Terminate GLFW
}

// Main loop to run the application
// Renders the black hole and handles frame timing
void App::run(BlackHole& blackhole) {

	lastTime = glfwGetTime(); // Initialize the last frame time
	numFrames = 0; // Initialize the frame counter
	frameTime = 16.0f; // Set the initial frame time

	std::vector<Ray> rays = InitializeRays(2, blackhole.r_s);

    while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen

		blackhole.Draw(shader); 

		Ray::Draw(rays, shader); 

		glfwSwapBuffers(window); // Swap the front and back buffers
		glfwPollEvents(); 

		handle_frame_timing(); // Handle frame timing and update the window title
	}
}

// Function to set up GLFW and create a window
void App::set_up_glfw() {

    glfwInit(); // Initialize GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Use core profile
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // Ensure compatibility
	
	window = glfwCreateWindow(800, 600, "Sagittarius A*", NULL, NULL); // Create a window
    if (!window) {
        std::cerr << "Failed to create GLFW window." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
	glfwMakeContextCurrent(window); // Make the window's context current
	glfwSwapInterval(1); // Enable V-Sync
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Hide the cursor

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {  // Checks if GLAD was able to load al OpenGL function pointers. 
		std::cerr << "Couldn't load OpenGL." << std::endl;
		glfwTerminate();
        exit(EXIT_FAILURE);
	}
}

// Function to set up OpenGL settings and load shaders
void App::set_up_opengl() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 

	// Set the rendering region to the actual screen size
	int w,h;
	glfwGetFramebufferSize(window, &w, &h); 
	glViewport(0,0,w,h); // Resolution

	glEnable(GL_DEPTH_TEST); // Ensures objects closer to the camera hide objects behind them 
	glDepthFunc(GL_LESS); 

	glEnable(GL_CULL_FACE); // Prevents rendering of faces that you don't see. 
    glCullFace(GL_BACK); 

    shader = make_shader(
		"../src/shaders/vertex.txt", 
		"../src/shaders/fragment.txt"); // Load and compile shaders
    if (!shader) {
        std::cerr << "Failed to create shader program." << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    glUseProgram(shader); 

    // Set projection matrix
	GLint projLocation = glGetUniformLocation(shader, "projection");
    if (projLocation == -1) {
        std::cerr << "Failed to find 'projection' uniform location." << std::endl;
    }
	glm::mat4 projection = glm::perspective(
		45.0f, 640.0f / 480.0f, 0.1f, 50.0f); // Create a perspective projection matrix
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(projection)); 

    // Set view matrix
    GLint viewLocation = glGetUniformLocation(shader, "view");
    if (viewLocation == -1) {
        std::cerr << "Failed to find 'view' uniform location." << std::endl;
    }
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f), // Look at origin
        glm::vec3(0.0f, 1.0f, 0.0f)  // Up vector
    );
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));

    // Set model matrix
    GLint modelLocation = glGetUniformLocation(shader, "model");
    if (modelLocation == -1) {
        std::cerr << "Failed to find 'model' uniform location." << std::endl;
    }
    glm::mat4 model = glm::mat4(1.0f); // Identity matrix
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
}

// Function to handle frame timing and update the window title with FPS
void App::handle_frame_timing() {
	currentTime = glfwGetTime(); // Get the current time
	double delta = currentTime - lastTime; // Calculate the time difference

	if (delta >= 1) {
		int framerate{ std::max(1, int(numFrames / delta)) }; // Calculate the framerate
		std::stringstream title;
		title << "Sagittarius A running at " << framerate << " fps.";
		glfwSetWindowTitle(window, title.str().c_str()); // Update the window title
		lastTime = currentTime; // Reset the last frame time
		numFrames = -1; // Reset the frame counter
		frameTime = float(1000.0 / framerate); // Update the frame time
	}

	++numFrames; // Increment the frame counter
}

std::vector<Ray> App::InitializeRays(int numRays, double r_s){
	std::vector<Ray> rays;
	rays.reserve(numRays);

	for (int i = 0; i < numRays; ++i){
		glm::vec2 pos;
		pos.x = -1.0f + ((float)rand() / RAND_MAX) * 0.5f; // -1.0 to -0.5 (left side)
		pos.y = -1.0f + ((float)rand() / RAND_MAX) * 2.0f;  // -1.0 to 1.0 (full height)

		glm::vec2 dir = glm::normalize(glm::vec2(1.0f, ((float)rand() / RAND_MAX) * 2.0f - 1.0f));

    	// Create the Ray and add to the vector
    	rays.emplace_back(pos, dir, r_s);
	}

	return rays; 
}