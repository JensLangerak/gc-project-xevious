#ifndef ENTITY_H
#define ENTITY_H

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


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "models.h"
#include "utils.h"
#include "entity.h"

class Entity
{
    public:
        Entity();
        virtual ~Entity();
        
        virtual void draw(long thick, glm::mat4 projView);
       // virtual void update(long thick);

	protected:
    //mesh
    
    //location etc information
    glm::vec3 position;
    glm::vec3 orientation;


    //collision

    private:
};

#endif // ENTITY_H