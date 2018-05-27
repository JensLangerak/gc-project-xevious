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
#include "bounding_box.h"

class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual void draw(long thick, glm::mat4 projView);
       // virtual void update(long thick);

    // @NOTE: don't want all models to be of equal size, but scale might not be the right tool
    float scale;
    glm::vec3 position;
    glm::vec3 orientation;
    glm::vec3 color = glm::vec3(1,1,1); //tijdelijk (Denk ik)
    models::ModelType model;

    models::Textures texture = models::Textures::None;
        
    // Gameplay related
    bool isCollidable;
    BoundingCube boundingCube;
    BoundingBox boundingBox;

    // @NOTE(Berend): This is needed (for debugging purposes) in public scope right now. Will fix later
    glm::mat4 getTransformationMatrix();
protected:
    // Mesh
    
    // Location etc information



    // Collision
    glm::vec2 get2DPosition();
    BoundingBox getLocatedBoundingBox();
    // @TODO: Add Collision check that only checks bounding boxes if collidable

private:
};

#endif // ENTITY_H
