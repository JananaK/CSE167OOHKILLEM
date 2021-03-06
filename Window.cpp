#include "window.h"

const char* window_title = "GLFW Starter Project";
//Cube * cube;
OBJObject * object;
GLint shaderProgram;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "/Users/janic/OneDrive/Documents/UCSD/UCSD hw and papers/Fall 2017/CSE167StarterCode2-master/CSE167StarterCode2-master/shader.vert"
#define FRAGMENT_SHADER_PATH "/Users/janic/OneDrive/Documents/UCSD/UCSD hw and papers/Fall 2017/CSE167StarterCode2-master/CSE167StarterCode2-master/shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

glm::vec3 Window::Translation(0, 0, 0);		// keeps track of location
glm::vec3 Window::Scale(1.0f, 1.0f, 1.0f);	// keeps track of scaling and orientation

int Window::width;
int Window::height;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	//cube = new Cube();
	//object = new OBJObject("bunny.obj");
	object = new OBJObject("bear.obj");
	//object = new OBJObject("dragon.obj");

	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	delete(object);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	// object->update();
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Use the shader of programID
	glUseProgram(shaderProgram);
	
	// Render the cube
	object->draw(shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if 'X' was pressed, moves right
		if (key == GLFW_KEY_X && mods & GLFW_MOD_SHIFT)
		{
			object->translate(glm::vec3(1.0f, 0, 0));
			Translation += glm::vec3(1.0f, 0, 0);
		}

		// Check if 'x' was pressed, moves left
		else if (key == GLFW_KEY_X)
		{
			object->translate(glm::vec3(-1.0f, 0, 0));
			Translation += glm::vec3(-1.0f, 0, 0);
		}

		// Check if 'Y' was pressed, moves up
		if (key == GLFW_KEY_Y && mods & GLFW_MOD_SHIFT)
		{
			object->translate(glm::vec3(0, 1.0f, 0));
			Translation += glm::vec3(0, 1.0f, 0);
		}

		// Check if 'y' was pressed, moves down
		else if (key == GLFW_KEY_Y)
		{
			object->translate(glm::vec3(0, -1.0f, 0));
			Translation += glm::vec3(0, -1.0f, 0);
		}

		// Check if 'Z' was pressed, moves into screen
		if (key == GLFW_KEY_Z && mods & GLFW_MOD_SHIFT)
		{
			object->translate(glm::vec3(0, 0, 1.0f));
			Translation += glm::vec3(0, 0, 1.0f);
		}

		// Check if 'z' was pressed, moves out of screen
		else if (key == GLFW_KEY_Z)
		{
			object->translate(glm::vec3(0, 0, -1.0f));
			Translation += glm::vec3(0, 0, -1.0f);
		}

		// Check if 'S' was pressed, scales up around object's center
		if (key == GLFW_KEY_S && mods & GLFW_MOD_SHIFT)
		{
			object->scale(glm::vec3(1.1f, 1.1f, 1.1f));
			Scale *= glm::vec3(1.1f, 1.1f, 1.1f);
		}

		// Check if 's' was pressed, scales down around object's center
		else if (key == GLFW_KEY_S)
		{
			object->scale(glm::vec3(0.9f, 0.9f, 0.9f));
			Scale *= glm::vec3(0.9f, 0.9f, 0.9f);

		}

		// Check if 'j' was pressed, center!
		if (key == GLFW_KEY_J)
		{
			object->centerScale();
		}

		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
	}
}
