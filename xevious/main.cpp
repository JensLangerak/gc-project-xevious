// Library for OpenGL function loading
// Must be included before GLFW
#define GLEW_STATIC
#include <GL/glew.h>

// Library for window creation and event handling
#include <GLFW/glfw3.h>

// Library for vertex and matrix math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "models.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"

// Configuration
const int WIDTH = 800;
const int HEIGHT = 600;


bool checkShaderErrors(GLuint shader) {
	// Check if the shader compiled successfully
	GLint compileSuccessful;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccessful);

	// If it didn't, then read and print the compile log
	if (!compileSuccessful) {
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

		std::vector<GLchar> logBuffer(logLength);
		glGetShaderInfoLog(shader, logLength, nullptr, logBuffer.data());

		std::cerr << logBuffer.data() << std::endl;
		
		return false;
	} else {
		return true;
	}
}


bool checkProgramErrors(GLuint program) {
	// Check if the program linked successfully
	GLint linkSuccessful;
	glGetProgramiv(program, GL_LINK_STATUS, &linkSuccessful);

	// If it didn't, then read and print the link log
	if (!linkSuccessful) {
		GLint logLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		std::vector<GLchar> logBuffer(logLength);
		glGetProgramInfoLog(program, logLength, nullptr, logBuffer.data());

		std::cerr << logBuffer.data() << std::endl;
		
		return false;
	} else {
		return true;
	}
}

// OpenGL debug callback
void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
		std::cerr << "OpenGL: " << message << std::endl;
	}
}

// Helper function to read a file like a shader
std::string readFile(const std::string& path) {
	std::ifstream file(path, std::ios::binary);
	
	std::stringstream buffer;
	buffer << file.rdbuf();

	return buffer.str();
}

Camera camera;
Camera mainLight;

int main(int argc, char** argv)
{
 if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW!" << std::endl;
		return EXIT_FAILURE;
	}

	// Create window and OpenGL 4.3 debug context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Xevious", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create OpenGL context!" << std::endl;
		return EXIT_FAILURE;
	}

	// Activate the OpenGL context
	glfwMakeContextCurrent(window);

	// Initialize GLEW extension loader
	glewExperimental = GL_TRUE;
	glewInit();

	// Set up OpenGL debug callback
	glDebugMessageCallback(debugCallback, nullptr);
    
    
    
    globals::mainProgram = glCreateProgram();
	

	////////////////// Load and compile main shader program
	{
		std::string vertexShaderCode = readFile("shader.vert");
		const char* vertexShaderCodePtr = vertexShaderCode.data();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
		glCompileShader(vertexShader);

		std::string fragmentShaderCode = readFile("shader.frag");
		const char* fragmentShaderCodePtr = fragmentShaderCode.data();

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
		glCompileShader(fragmentShader);

		if (!checkShaderErrors(vertexShader) || !checkShaderErrors(fragmentShader)) {
			std::cerr << "Shader(s) failed to compile!" << std::endl;
			std::cout << "Press enter to close."; getchar();
			return EXIT_FAILURE;
		}

		// Combine vertex and fragment shaders into single shader program
		glAttachShader(globals::mainProgram, vertexShader);
		glAttachShader(globals::mainProgram, fragmentShader);
		glLinkProgram(globals::mainProgram);

		if (!checkProgramErrors(globals::mainProgram)) {
			std::cerr << "Main program failed to link!" << std::endl;
			std::cout << "Press enter to close."; getchar();
			return EXIT_FAILURE;
		}
	}

	// Load vertices of model
    if (!models::loadModels())
    {
        std::cerr << "Program failed to load!" << std::endl;
		return EXIT_FAILURE;   
    }
    
    camera.aspect = WIDTH / (float)HEIGHT;
	camera.position = glm::vec3(0.f, 1.5f, 1.0f);
	camera.forward  = -camera.position;
    
    
    mainLight.aspect = WIDTH / (float)HEIGHT;
	mainLight.position = glm::vec3(-10.f, 10.f, 8.9f);
	mainLight.forward  = -mainLight.position;
    
    // Enable depth testing
	glEnable(GL_DEPTH_TEST);

    Entity player;
    Entity other;
    player.model = models::ModelType::Dragon;
    player.color = glm::vec3(1.,0.,0.);
    other.color = glm::vec3(0.,1.,0.);
    other.model = models::ModelType::Dragon;
    
	while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
        
    
        //update();
        //checkCollisions();
        
        //draw();
        
        
        // Bind the shader
		glUseProgram(globals::mainProgram);
		updateCamera(camera); //misschien niet nodig
        
        glm::mat4 vp = camera.vpMatrix();

	//	glUniformMatrix4fv(glGetUniformLocation(mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

        glm::mat4 lightMVP = mainLight.vpMatrix();
        glUniform3fv(glGetUniformLocation(globals::mainProgram, "lightPos"), 1, glm::value_ptr(mainLight.position));  
  
        // Set view position
		glUniform3fv(glGetUniformLocation(globals::mainProgram, "viewPos"), 1, glm::value_ptr(camera.position));

        glClearDepth(1.0f);  
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        player.draw(0,vp);
        other.draw(0,vp);
        other.position[0] += 0.001;
        other.orientation[1] += 0.001;
        
        glfwSwapBuffers(window);
        //sleep();
        
    }
    
        glfwDestroyWindow(window);
	
	glfwTerminate();
    
    return 0;
}
