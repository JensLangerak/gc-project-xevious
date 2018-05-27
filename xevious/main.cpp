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

#include "mesh_simplification.h"

#include "player_entity.h"


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

		std::vector<GLchar> logBuffer(logLength+1);
		glGetProgramInfoLog(program, logLength, nullptr, logBuffer.data());
		logBuffer[logLength] = '\0';

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

// @TODO: Move to globals
PlayerEntity player;

void setupDebugging()
{
	// 1. Setup for drawing bounding boxes
	// Create VBO and VAO for boundingBox objects;
	glCreateBuffers(1, &globals::boundingBoxVBO);
	glGenVertexArrays(1, &globals::boundingBoxVAO);

	// Set up Vertex array
	glBindVertexArray(globals::boundingBoxVAO); 
	
	glBindBuffer(GL_ARRAY_BUFFER, globals::boundingBoxVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
	glEnableVertexAttribArray(0);

	// @NOTE: Perhaps this can be completely removed
	glBindBuffer(GL_ARRAY_BUFFER, globals::boundingBoxVBO);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	// Bind default buffer again
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// 2. Setup debug program for shading bounding boxes
	globals::debugProgram = glCreateProgram();
	std::string vertexShaderCode = readFile("shaders/boundingBoxDebug.vert");
	const char* vertexShaderCodePtr = vertexShaderCode.data();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
	glCompileShader(vertexShader);

	std::string fragmentShaderCode = readFile("shaders/boundingBoxDebug.frag");
	const char* fragmentShaderCodePtr = fragmentShaderCode.data();
 
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
	glCompileShader(fragmentShader);

	if (!checkShaderErrors(vertexShader) || !checkShaderErrors(fragmentShader)) {
		std::cerr << "debug Shader(s) failed to compile!" << std::endl;
		std::cout << "Press enter to close."; getchar();
		return;
	}

	// Combine vertex and fragment shaders
	glAttachShader(globals::debugProgram, vertexShader);
	glAttachShader(globals::debugProgram, fragmentShader);
	glLinkProgram(globals::debugProgram);

	if (!checkProgramErrors(globals::debugProgram)) {
		std::cerr << "Debug program failed to link!" << std::endl;
		std::cout << "Press enter to close."; getchar();
		return;
	}
	// @TODO: Perhaps create a destroyDebugging() if necessary
}

void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// @NOTE: Forward and backward are flipped, because depth grows into -z direction
	// @TEST: Test moving bounding box1
	if (key == GLFW_KEY_W)
	{
		// testBbox.topLeft.y += 0.05;
		player.performAction(PlayerAction::MOVE_FORWARD);
	}
	else if (key == GLFW_KEY_A)
	{
		// testBbox.topLeft.x -= 0.05;
		player.performAction(PlayerAction::MOVE_LEFT);
	}
	else if (key == GLFW_KEY_S)
	{
		// testBbox.topLeft.y -= 0.05;
		player.performAction(PlayerAction::MOVE_BACKWARD);
	}
	else if (key == GLFW_KEY_D)
	{
		// testBbox.topLeft.x += 0.05;
		player.performAction(PlayerAction::MOVE_RIGHT);
	}

}

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

	// Attach keyboard and mouse handlers
	glfwSetKeyCallback(window, handleKeyboard);

	// Activate the OpenGL context
	glfwMakeContextCurrent(window);

	// Initialize GLEW extension loader
	glewExperimental = GL_TRUE;
	glewInit();

	// Set up OpenGL debug callback
	glDebugMessageCallback(debugCallback, nullptr);    

    setupDebugging();


    globals::mainProgram = glCreateProgram();
	////////////////// Load and compile main shader program
	{
		std::string vertexShaderCode = readFile("shaders/shader.vert");
		const char* vertexShaderCodePtr = vertexShaderCode.data();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
		glCompileShader(vertexShader);

		std::string fragmentShaderCode = readFile("shaders/shader.frag");
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

    // @TODO: possibly change to top-down orthographic camera?
    models::loadTextures();
    camera.aspect = WIDTH / (float)HEIGHT;
	camera.position = glm::vec3(0.f, 1.5f, 1.0f);
	camera.forward  = -camera.position;
  
    mainLight.aspect = WIDTH / (float)HEIGHT;
	mainLight.position = glm::vec3(-30.f, 100.f, 10.f);
	mainLight.forward  = -mainLight.position;
    
    // Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// @TEST: this is just used for testing;
 	// @TODO: Factor player setup into seperate function
    player.model = models::ModelType::PlayerShip;
    player.color = glm::vec3(1.,0.,0.);
    player.scale = 0.05;
    player.boundingCube = models::makeBoundingCube(models::playerShip.vertices);
    
    Entity other;
    other.model = models::ModelType::StarEnemy;
    other.color = glm::vec3(0.,1.,0.);
    other.scale = 0.05;
    other.boundingCube = models::makeBoundingCube(models::starEnemy.vertices);


    // @TODO: Make this work on windows
    models::generateTerrain(30, 30, 100, 100);
    
    Entity terrain;
    terrain.model = models::ModelType::Terrain;
    terrain.texture = models::Textures::Sand;
    terrain.position =  glm::vec3(0.,-10.,-10.);

    MeshSimplification simple = MeshSimplification(models::dragon.vertices, 10);
    models::loadSimple(simple.simplifiedMesh);
//    other.model = models::ModelType::Simple;


    player.boundingCube.print();


	while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();



		// Update section
		{
			// @NOTE: Should a "input-struct" be passed to the update functions / be globally defined?

			// @TODO: Implement
			// Iterate over entity list and update each entity;

			// Delete entities marked dead

			// @NOTE: Perhaps perform global update() if necessary
		}

		// Render section
    	{
    		// @NOTE: Do we need multiple shaders?
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
	        glClearColor(0.f, 0.f, 0.f, 1.0f);
	        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	        // @NOTE: Refactor into section that renders entity list
	        player.draw(0,vp);
	        other.draw(0,vp);
	        other.position[0] += 0.001;
	        other.orientation[1] += 0.001;

	        // @TODO: Make work
	        terrain.draw(0, vp);



	        // ====================== debug render section =======================
	        // Set debug program shader
			glUseProgram(globals::debugProgram);

	        // @TEST: Draw bounding box
	        glm::vec3 hitColor = glm::vec3(1.0, 0.0, 0.0);
	        glm::vec3 normColor = glm::vec3(0.0, 0.0, 1.0);

	        // @TODO: Implement BoundingCube -> BoundingBox

	        // @TEST: Draw bounding box
	        BoundingBox otherBbox = other.boundingCube.getProjectedBoundingBox(other.getTransformationMatrix());
	        BoundingBox playerBbox = player.boundingCube.getProjectedBoundingBox(player.getTransformationMatrix());

	        if (otherBbox.checkIntersection(playerBbox))
	        {
	        	// Draw projected bounding box in hit
		        player.boundingCube.draw(vp * player.getTransformationMatrix(), hitColor);
		        other.boundingCube.draw(vp * other.getTransformationMatrix(), hitColor);

	        } else
	        {
		        player.boundingCube.draw(vp * player.getTransformationMatrix(), normColor);
		        other.boundingCube.draw(vp * other.getTransformationMatrix(), normColor);
	        }

			//simple.drawGrid(vp * other.getTransformationMatrix());
	        glfwSwapBuffers(window);
	        //sleep();
    	}
    }
    
    glfwDestroyWindow(window);
	
	glfwTerminate();
    
    return 0;
}
