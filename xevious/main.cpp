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
#include <ctime>

#include "models.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"
#include "player_entity.h"
#include "enemy_entity.h"

// Configuration
const int WIDTH = 800;
const int HEIGHT = 600;

Gamestate gamestate;

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

// @TODO: Move to globals or some kind of GameState struct
PlayerEntity* player;

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

	std::cout << "Xevious: Press P to enable debug mode\n";
}

void handleKeyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// @NOTE: Forward and backward are flipped, because depth grows into -z direction
	// @TEST: Test moving bounding box1
	if (action != GLFW_PRESS && action != GLFW_REPEAT)
	{	
		// @TODO(Berend): change into something more smooth;
		return;
	}

	if (key == GLFW_KEY_W)
	{
		player->performAction(PlayerAction::MOVE_FORWARD);
	}
	else if (key == GLFW_KEY_A)
	{
		player->performAction(PlayerAction::MOVE_LEFT);
	}
	else if (key == GLFW_KEY_S)
	{
		player->performAction(PlayerAction::MOVE_BACKWARD);
	}
	else if (key == GLFW_KEY_D)
	{
		player->performAction(PlayerAction::MOVE_RIGHT);
	} else if (key == GLFW_KEY_SPACE)
	{
		player->performAction(PlayerAction::SHOOT);
	} else if (key == GLFW_KEY_Q)
	{
		player->performAction(PlayerAction::ROLL);
	} else if (key == GLFW_KEY_P)
	{
		// Toggle debug mode
		globals::debugMode = !globals::debugMode;
	}
	// @TEST: Rotating cannon, remove once done testing
	else if (key == GLFW_KEY_T)
	{
		player->weaponAngle -= 0.1;
	} else if (key == GLFW_KEY_Y)
	{
		player->weaponAngle += 0.1;
	}

	else if (key == GLFW_KEY_Z)
	{
		player->position.y += 0.05;
	} else if (key == GLFW_KEY_X)
	{
		player->position.y -= 0.1;
	}
}


// @TODO(Bug): Fix 
void updateMouse(GLFWwindow* window, glm::mat4 vp, Gamestate* gamestate)
{
	// Calculate current position
	glm::vec4 pos = gamestate->player->getScreenPosition(vp);

	// @TODO: fix
	glm::vec2 cannonPos = glm::vec2(pos.x, pos.y);
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	std::cout << "Mouse x, y: " << mouseX << ", " << mouseY << "\n";
	std::cout << "Projected " << cannonPos.x << ", " << cannonPos.y << "\n";

	glm::vec2 dir = glm::normalize(glm::vec2((mouseX / WIDTH) * 4 - 2, (mouseY / HEIGHT) * 4 - 2) - cannonPos);

	// Extract angle
	float angle = atan(dir.y / dir.x);
	// @TODO: Fix offset
	gamestate->player->weaponAngle = angle - 3.14/2;
}


// @TODO: Factor into different file / class when this becomes too large 
//		or we need to switch AI's (for different levels etc)
void veryObviousAI(Gamestate* state, double delta)
{
	// Every second, release a new Enemy entity into the world
	if (state->aiTimer <= 0)
	{
		// Generate random x position between -1 and 1
		float randVal = (float) rand() / (float) RAND_MAX;
		float randX = randVal * 2. - 1.;

		EnemyEntity* enemy = new EnemyEntity(glm::vec2(randX, -1.));
		state->entityList->push_back(enemy);

		// Reset timer
		state->aiTimer = 1000.0;
	} else
	{
		state->aiTimer -= delta;
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

	// ================== Setup Gamestate ================
    // @NOTE(Dirty): This is a dirty dirty hack
    // Reinitialize player to correctly calculate bounding cube
	gamestate.player = new PlayerEntity();
	gamestate.entityList = new std::vector<Entity*>();

	player = gamestate.player;
	gamestate.entityList->push_back(player);

    EnemyEntity* testEnemy = new EnemyEntity();
    testEnemy->position = glm::vec3(.0, .0, -1.);
    testEnemy->model = models::ModelType::StarEnemy;
    testEnemy->color = glm::vec3(0., 1., 1.);
    testEnemy->scale = 0.05;
    testEnemy->boundingCube = models::makeBoundingCube(models::starEnemy.vertices);
    gamestate.entityList->push_back(testEnemy);

    // @TODO: Make this work on windows (msvc doesn't like nonstandard c++)
    models::generateTerrain(30, 30, 100, 100);
    
    Entity terrain;
    terrain.model = models::ModelType::Terrain;
    terrain.texture = models::Textures::Sand;
    terrain.position =  glm::vec3(0.,-10.,-10.);

    clock_t timeStartFrame = clock();
    clock_t timeEndFrame = clock();

	while (!glfwWindowShouldClose(window)) 
    {
        glfwPollEvents();
       	timeStartFrame = timeEndFrame;
        timeEndFrame = clock();
        double timeDelta = difftime(timeEndFrame, timeStartFrame);
        //std::cout << "Timedelta: " << timeDelta << "\n"; 
        //std::cout << "FPS: " << timeDelta << std::endl; 
		
		// Update section
		{
			// ====================== update entities ========================
			// Iterate over entity list and update each entity;
			for (int i = 0; i < gamestate.entityList->size(); ++i)
        	{
        		Entity* e = (*gamestate.entityList)[i];
        		e->debugIsColliding = false;
        		
        		// @NOTE: Can be removed once delete entities on death is implemented
        		if (!e->canBeRemoved)
        		{
        			e->update(timeDelta, &gamestate);	
        		}
        	}

        	// ====================== filter dead entities ========================
			// @TODO: Delete entities marked dead

        	// ====================== collision detection ========================
			// Not perfect, but good enough
	
			// @TODO(BUG): different enemies should not die from each others hits
			for (int j = 0; j < gamestate.entityList->size(); ++j)
	        {
	        	Entity* a = (*gamestate.entityList)[j];
	        	Entity* player = gamestate.player;

	        	if (a->isCollidable && a != player && player->getProjectedBoundingBox().checkIntersection(a->getProjectedBoundingBox()))
	        	{
	        			a->debugIsColliding = true;
	        			player->debugIsColliding = true;
	        			a->onCollision(player);
						player->onCollision(a);
	        	}
	        }        		
        	
        	// ====================== Run AI ========================
        	veryObviousAI(&gamestate, timeDelta);
        	// update mouse position
        	updateMouse(window, camera.vpMatrix(), &gamestate);
		}

		// Render section
    	{
			// ====================== game render section ========================
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
	        for (int i = 0; i < gamestate.entityList->size(); ++i)
	        {
	        	(*gamestate.entityList)[i]->draw(0, vp);
	        }

	        // @TODO: Make work
	        terrain.draw(0, vp);

	        // ====================== debug render section =======================
	        if (globals::debugMode)
	        {
		        // Set debug program shader
				glUseProgram(globals::debugProgram);

		        glm::vec3 hitColor = glm::vec3(1.0, 0.0, 0.0);
		        glm::vec3 normColor = glm::vec3(0.0, 0.0, 1.0);

				for (int i = 0; i < gamestate.entityList->size(); ++i)
	        	{
	        		Entity* e = (*gamestate.entityList)[i];
	        		if (e->debugIsColliding)
	        		{
	        			e->drawBoundingCube(vp, hitColor);
	        		}
	        		else 
	        		{
	        			e->drawBoundingCube(vp, normColor);
	        		}
	        	}
	        }

	        glfwSwapBuffers(window);
	        //sleep();
    	}
    }
    
    glfwDestroyWindow(window);
	
	glfwTerminate();
    
    return 0;
}
