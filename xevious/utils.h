#ifndef UTILS_H
#define UTILS_H

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
#include <list>

// Forward declaring PlayerEntity and Entity
class PlayerEntity;
class Entity;
class BulletEntity;

// @TODO: Implement
//enum class Gamemode

struct Gamestate
{
	PlayerEntity* player;
	std::list<Entity*>* entityList;
	std::list<BulletEntity*>* bulletList;
	double aiTimer;
};

// Per-vertex data
struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;
};


namespace globals {
	extern bool debugMode;
    extern GLuint mainProgram;

    extern GLuint debugProgram;
    extern GLuint boundingBoxVBO;
    extern GLuint boundingBoxVAO;
}

glm::mat4 getRotationMatrix(double xRot ,double yRot ,double zRot);
glm::mat4 getScalingMatrix(double scale);

#endif