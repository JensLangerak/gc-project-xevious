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
#include <ctime>
#include <cmath>
#include <list>
#include <vector>

#include "models.h"
#include "utils.h"
#include "entity.h"
#include "camera.h"

#include "mesh_simplification.h"

#include "player_entity.h"
#include "enemy_entity.h"
#include "bullet_entity.h"
#include "boss_entity.h"
#include "terrain_generator.h"
#include <chrono>
// using std::list;
using std::vector;

// Configuration
const int WIDTH = 800;
const int HEIGHT = 800;
const int SHADOWTEX_WIDTH  = 1024;
const int SHADOWTEX_HEIGHT = 1024;

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
void APIENTRY debugCallback(GLenum source , GLenum type , GLuint id ,
	GLenum severity, GLsizei length , const GLchar* message, const void* userParam ) {
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

void handleKeyboard(GLFWwindow* window , int key, int scancode , int action, int mods )
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
		player->performAction(PlayerAction::MOVE_FORWARD, &gamestate);
	}
	else if (key == GLFW_KEY_A)
	{
		player->performAction(PlayerAction::MOVE_LEFT, &gamestate);
	}
	else if (key == GLFW_KEY_S)
	{
		player->performAction(PlayerAction::MOVE_BACKWARD, &gamestate);
	}
	else if (key == GLFW_KEY_D)
	{
		player->performAction(PlayerAction::MOVE_RIGHT, &gamestate);
	} else if (key == GLFW_KEY_SPACE)
	{
		player->performAction(PlayerAction::SHOOT, &gamestate);
	} else if (key == GLFW_KEY_Q)
	{
		player->performAction(PlayerAction::ROLL, &gamestate);
	} else if (key == GLFW_KEY_P)
	{
		// Toggle debug mode
		globals::debugMode = !globals::debugMode;
	} else if (key == GLFW_KEY_0)
	{
		// @TODO: Implement
		// Reset gamestate!!
		// resetGameplay();
	}
}

void handleMouseButtons(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
    	player->performAction(PlayerAction::SHOOT, &gamestate);
    }
}


// @TODO(Bug): fix
void updateMouse(GLFWwindow* window, glm::mat4 vp, Gamestate* gamestate)
{
	// Calculate current position
	glm::vec4 pos = gamestate->player->getScreenPosition(vp);

	glm::vec2 cannonPos = glm::vec2(pos.x / pos.w, pos.y / pos.w);
	double mouseX;
	double mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	glm::vec2 cannonAbs = glm::vec2((cannonPos.x + 1.0) / 2 * WIDTH, (-cannonPos.y + 1.0) / 2 * HEIGHT);
	glm:: vec2 dir = glm::normalize(glm::vec2(mouseX, mouseY) - cannonAbs);


	// Extract angle
	float angle = atan2(dir.y, dir.x);
	gamestate->player->weaponAngle = angle + 3.14/2;
}

void veryObviousAI(Gamestate* state, double delta)
{
	if (state->stage == 0 && !state->stageReady)
	{
		//  ================ Init level stage ================
		// @TODO: Implement in case the gamestate has been reset
		state->stageReady = true;
	} 
	else if (state->stage == 0 && state->stageReady)
	{
		//  ================ run level stage ================
		// Every second, release a new Enemy entity into the world
		state->stageTimer -= delta;
		if (state->aiTimer <= 0)
		{
			// Generate random x position between -1 and 1
			float randVal = (float) rand() / (float) RAND_MAX;
			float randX = randVal * 2. - 1.;

			EnemyEntity* enemy = new EnemyEntity(glm::vec2(randX, -1.5));
			state->entityList->push_back(enemy);

			// Reset timer
			state->aiTimer = 1.0;
		} else
		{
			state->aiTimer -= delta;
		}

		if (state->stageTimer <= 0)
		{
			state->stage = 1;
			state->stageReady = false;
		}
	} 
	else if (state->stage == 1 && !state->stageReady)
	{
		// Init boss stage
		BossEntity* boss = new BossEntity();
		state->entityList->push_back(boss);
		state->stageReady = true;
	} 
	else if (state->stage == 1 && state->stageReady)
	{
		// Perform boss-fight code
	}
}

bool createProgram(GLuint & program, const char *filenameVertex,  const char *filenameFragment)
{
	std::string vertexShaderCode = readFile(filenameVertex);
	const char* vertexShaderCodePtr = vertexShaderCode.data();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderCodePtr, nullptr);
	glCompileShader(vertexShader);

	std::string fragmentShaderCode = readFile(filenameFragment);
	const char* fragmentShaderCodePtr = fragmentShaderCode.data();

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderCodePtr, nullptr);
	glCompileShader(fragmentShader);

	if (!checkShaderErrors(vertexShader) || !checkShaderErrors(fragmentShader)) {
		std::cerr << "Shader(s) failed to compile!" << std::endl;
		std::cout << "Press enter to close.";
		getchar();
		return false;
	}

    // Combine vertex and fragment shaders into single shader program
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	if (!checkProgramErrors(program)) {
		std::cerr << "Main program failed to link!" << std::endl;
		std::cout << "Press enter to close."; getchar();
		return false;
	}

	return true;
}

GLuint createTexture(){
	GLuint texShadow;

	glGenTextures(1, &texShadow);
	glActiveTexture( GL_TEXTURE0 + texShadow);
	glBindTexture(GL_TEXTURE_2D, texShadow);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Set behaviour for when texture coordinates are outside the [0, 1] range
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Set interpolation for texture sampling (GL_NEAREST for no interpolation)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	return texShadow;
}

bool createFramebuffer(GLuint &framebuffer, GLuint texture) {
//////////////////// Create framebuffer for extra texture
	glGenFramebuffers(1, &framebuffer);

/////////////////// Set shadow texure as depth buffer for this framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	return true;
}

void calculateShadowMap(GLuint &framebuffer, Camera & light)
{
    // Bind the off-screen framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Clear the shadow map and set needed options
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

    // Bind the shader
	glUseProgram(globals::shadowProgram);


    // Set viewport size
	glViewport(0, 0, SHADOWTEX_WIDTH, SHADOWTEX_HEIGHT);
	glm::mat4 vp = light.vpMatrix();

	glUniformMatrix4fv(glGetUniformLocation(globals::shadowProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(vp));


    // Set view position
	glUniform3fv(glGetUniformLocation(globals::shadowProgram, "viewPos"), 1, glm::value_ptr(light.position));

    // Execute draw command
	for (unsigned int i = 0; i < gamestate.entityList->size(); ++i)
	{
		(*gamestate.entityList)[i]->draw(0, vp);
	}
    //    terrain.draw(0, vp);

    // Unbind the off-screen framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render(GLuint texShadow, Camera &camera, vector<Entity*> &entityList, vector<BulletEntity*> & bulletList, TerrainGenerator &terrain)
{
    // ====================== game render section ========================
    // Bind the shader
	glUseProgram(globals::mainProgram);
	glViewport(0, 0, WIDTH, HEIGHT);
	glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0 + texShadow);
	glBindTexture(GL_TEXTURE_2D, texShadow);
	glUniform1i(glGetUniformLocation(globals::mainProgram, "texShadow"), texShadow);

	glm::mat4 vp = camera.vpMatrix();

    //  glUniformMatrix4fv(glGetUniformLocation(mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));


	glm::mat4 lightMVP = mainLight.vpMatrix();
	glUniform3fv(glGetUniformLocation(globals::mainProgram, "lightPos"), 1, glm::value_ptr(mainLight.position));
	glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "lightMVP"), 1, GL_FALSE, glm::value_ptr(lightMVP));

    // Set view position
	glUniform3fv(glGetUniformLocation(globals::mainProgram, "viewPos"), 1, glm::value_ptr(camera.position));

	glClearDepth(1.0f);
	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // @NOTE: Refactor into section that renders entity list
	for (vector<Entity*>::iterator it = entityList.begin(); it != entityList.end(); it++)
	{
		(*it)->draw(0, vp);
	}

	terrain.drawChunks(0, vp);


	for (vector<BulletEntity*>::iterator it = bulletList.begin(); it != bulletList.end(); it++)
	{
		(*it)->draw(0, vp);
	}

    // ====================== debug render section =======================
	if (globals::debugMode)
	{
        // Set debug program shader
		glUseProgram(globals::debugProgram);

		glm::vec3 hitColor = glm::vec3(1.0, 0.0, 0.0);
		glm::vec3 normColor = glm::vec3(0.0, 0.0, 1.0);

		for (vector<Entity*>::iterator it = entityList.begin(); it != entityList.end(); it++)
		{
			Entity* e = *it;
			if (e->debugIsColliding)
			{
				e->drawBoundingCube(vp, hitColor);
			}
			else
			{
				e->drawBoundingCube(vp, normColor);
			}
		}

		for (vector<BulletEntity*>::iterator it = bulletList.begin(); it != bulletList.end(); it++)
		{
			BulletEntity* bullet = *it;
			if (bullet->debugIsColliding)
			{
				bullet->drawBoundingCube(vp, hitColor);
			}
			else
			{
				bullet->drawBoundingCube(vp, normColor);
			}
		}
	}

}


void updateGame(GLFWwindow* window, Gamestate &gamestate, double timeDelta)
{
	vector<Entity*>* entityList = gamestate.entityList;
	vector<BulletEntity*>* bulletList = gamestate.bulletList; 

	for (vector<Entity*>::iterator it = entityList->begin(); it != entityList->end();)
	{
		Entity* e = *it;
		if (e->canBeRemoved || !e->getProjectedBoundingBox().checkIntersection(gamestate.gamebox)) 
		{
			it = entityList->erase(it);
			delete e;
		} else {
			++it;
		}     
	}           

	for (vector<BulletEntity*>::iterator it = bulletList->begin(); it != bulletList->end();)
	{
		BulletEntity* bullet = *it;
		if (bullet->canBeRemoved || !bullet->getProjectedBoundingBox().checkIntersection(gamestate.gamebox))
		{
			it = bulletList->erase(it);
			delete bullet;
		} else
		{
			++it;
		}
	}

	// ====================== update entities & remove dead/outofbounds ========================
	int entityListSize = entityList->size();
	for (int i = 0; i < entityListSize; i++)
	{
		Entity* e = (*entityList)[i];
		e->debugIsColliding = false;
		e->update(timeDelta, &gamestate);		
	}

	int bulletListSize = bulletList->size();
	for (int i = 0; i < bulletListSize; ++i)
	{
		BulletEntity* bullet = (*bulletList)[i];
		bullet->update(timeDelta, &gamestate);
	}   

	// ====================== collision detection ========================
	// Not perfect, but good enough for now
	for (vector<Entity*>::iterator it = entityList->begin(); it != entityList->end(); ++it)
	{
		Entity* a = *it;
		Entity* player = gamestate.player;

			// @TODO: Do bullets have the correct bounding cubes?
		if (a->isCollidable && a != player && a->checkCollision(player)) // @IMPORTANT: order is important because of overloading functions
		{
			a->debugIsColliding = true;
			player->debugIsColliding = true;
			a->onCollision(player);
			player->onCollision(a);
		}
	}

	for (vector<BulletEntity*>::iterator bulletIt = bulletList->begin(); bulletIt != bulletList->end(); bulletIt++)
	{
		for (vector<Entity*>::iterator entityIt = entityList->begin(); entityIt != entityList->end(); entityIt++)
		{
			Entity* a = *entityIt;
			BulletEntity* b = *bulletIt;

			// @TODO(Dirty): Ugly hack to check for player here
			if (a->type != EntityType::Player 
				&& a->isCollidable 
				&& b->isCollidable
					&& a->checkCollision(b))	// @IMPORTANT: order is important because of overloading functions
			{
				a->debugIsColliding = true;
				b->debugIsColliding = true;
				a->onCollision(b);
				b->onCollision(a);
			}
		}
	}

	// ====================== Run AI ========================
	veryObviousAI(&gamestate, timeDelta);
	// update mouse position
	updateMouse(window, camera.vpMatrix(), &gamestate);
}

// @TODO: Implement once menu drawing is done
void updateMenu(GLFWwindow* window, Gamestate &gamestate, double timeDelta)
{

}

void update(GLFWwindow* window, Gamestate &gamestate, double timeDelta)
{
	if (gamestate.mode == GameMode::Playing)
	{
		updateGame(window, gamestate, timeDelta);
	}
}


int main(int argc , char** argv )
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

	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Xevious", nullptr, nullptr);
	if (!window) {
		std::cerr << "Failed to create OpenGL context!" << std::endl;
		return EXIT_FAILURE;
	}

	// Attach keyboard and mouse handlers
	glfwSetKeyCallback(window, handleKeyboard);
	glfwSetMouseButtonCallback(window, handleMouseButtons);

	// Activate the OpenGL context
	glfwMakeContextCurrent(window);

	// Initialize GLEW extension loader
	glewExperimental = GL_TRUE;
	glewInit();

	// Set up OpenGL debug callback
	glDebugMessageCallback(debugCallback, nullptr);    

	setupDebugging();

	globals::mainProgram = glCreateProgram();
	if (!createProgram(globals::mainProgram, "shaders/shader.vert", "shaders/shader.frag"))
		return EXIT_FAILURE;


	globals::shadowProgram = glCreateProgram();
	if (!createProgram(globals::shadowProgram, "shaders/shadow.vert", "shaders/shadow.frag"))
		return EXIT_FAILURE;


	//////////////////// Create Shadow Texture
	GLuint texShadow = createTexture();

	//////////////////// Create framebuffer for extra texture
	GLuint framebuffer = 0;
	if (!createFramebuffer(framebuffer, texShadow)) {
		return false;
	}

	// Load vertices of model
	if (!models::loadModels())
	{
		std::cerr << "Program failed to load!" << std::endl;
		return EXIT_FAILURE;   
	}

	models::loadTextures();


	camera.aspect = WIDTH / (float)HEIGHT;
	camera.position = glm::vec3(.0f,2.f, -0.0f); //TODO fix should be 0,2,0
	camera.forward = glm::vec3(0.f, -1.0f, 0.f);
	camera.up = glm::vec3(0.f,0.f,-1.f);
	//camera.forward  = -camera.position;
	camera.useOrthogonal = true;
	camera.width = 2.3;
	camera.height = 2.3;

	mainLight.aspect = WIDTH / (float)HEIGHT;
	mainLight.position = glm::vec3(-2.f,100.5f, -2.1f);
	mainLight.forward = glm::vec3(2.3f,-100.f, 1.6f);
	mainLight.up = glm::vec3(-0.f, 0.0f, -5.0);
	mainLight.far = 120.f;
	mainLight.near = 90.01;
    mainLight.useOrthogonal = true;//simulate far away;
    mainLight.width = 3.5;
    mainLight.height = 3.5;

	mainLight.fov = glm::pi<float>() / 70.f; //todo position light

    // Enable depth testing

	glEnable(GL_DEPTH_TEST);

	// ================== Setup Gamestate ================
	// @NOTE(Dirty): This is a dirty dirty hack
	// Reinitialize player to correctly calculate bounding cube
	gamestate.player = new PlayerEntity();
	gamestate.entityList = new std::vector<Entity*>();
	gamestate.bulletList = new std::vector<BulletEntity*>();
	// @TODO: Fix?
	gamestate.gamebox = BoundingBox(-1.5, -5.5, 3., 10.);

	player = gamestate.player;
	gamestate.entityList->push_back(player);


	// @TODO: Make this work on windows (msvc doesn't like nonstandard c++)

	
//	Entity terrain;
//	terrain.model = models::ModelType::Terrain;
//	terrain.texture = models::Textures::Sand;
//	terrain.position =  glm::vec3(0.,-3.5,0.);

    TerrainGenerator terrainGenerator(2.5,2.5/2.);


    auto timeStartFrame = std::chrono::system_clock::now();
    auto timeEndFrame = std::chrono::system_clock::now();
	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();
		timeStartFrame = timeEndFrame;
        timeEndFrame = std::chrono::system_clock::now();

		double timeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(timeEndFrame - timeStartFrame).count() / 1000.0f;

		        // std::cout << "FPS: " << 1./timeDelta << std::endl;

		// @NOTE: For convenience sake
		update(window, gamestate, timeDelta);


        terrainGenerator.UpdateChunks(timeDelta);


        calculateShadowMap(framebuffer, mainLight);

        //	glfwSwapBuffers(window);

//continue;
        render(texShadow, camera, *(gamestate.entityList), *(gamestate.bulletList),terrainGenerator);

        //simple.drawGrid(vp * other.getTransformationMatrix());
        glfwSwapBuffers(window);


        //sleep();
	}

	
	glfwDestroyWindow(window);
	
	glfwTerminate();
	
	return 0;
}
