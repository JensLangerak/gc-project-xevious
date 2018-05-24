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

// Per-vertex data
struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
};

namespace globals {
    extern GLuint mainProgram;
}

#endif