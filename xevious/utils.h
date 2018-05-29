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
#include <vector>
#include "bounding_box.h"

// Forward declaring necessary classes
class PlayerEntity;
class Entity;
class BulletEntity;

enum class GameMode
{
	Menu,
	Playing,
	Dead
};

struct Gamestate
{
	PlayerEntity* player;
	std::vector<Entity*>* entityList;
	std::vector<BulletEntity*>* bulletList;
	BoundingBox gamebox = BoundingBox(-1.5, -5.5, 3., 10.);;
	double aiTimer;
	double stageTimer = 10.;
	int stage = 0;
	bool stageReady = false;
	GameMode mode = GameMode::Playing; // @TODO: switch to Menu once implemented
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
	extern GLuint shadowProgram;
    extern GLuint debugProgram;
    extern GLuint boundingBoxVBO;
    extern GLuint boundingBoxVAO;
}

glm::mat4 getTranslationMatrix(glm::vec3 vecTranslation);
glm::mat4 getRotationMatrix(double xRot ,double yRot ,double zRot);
glm::mat4 getScalingMatrix(double scale);

#endif