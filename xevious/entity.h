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

enum class EntityType
{
    Player,
    Enemy,
    Bullet,
    None
};

class Entity
{
public:
    Entity();
    Entity(glm::vec3 col);
    virtual ~Entity();

    virtual void draw(long tick, glm::mat4 projView);
    virtual void drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor);    

    virtual void update(double tick, Gamestate* state);
    virtual void onCollision(Entity* entity);
    virtual BoundingBox getProjectedBoundingBox();      // @NOTE: Might eventually be replaced by checkCollision() method 

    // ============= Rendering related ============
    float scale = 1.;
    glm::vec3 color = glm::vec3(1,1,1); //tijdelijk (Denk ik)
    models::ModelType model;
    models::Textures texture = models::Textures::None;

    glm::vec3 orientation;
    glm::vec3 position;
    
    // ============= Gameplay related ============
    EntityType type = EntityType::None;
    bool canBeRemoved = false;
    bool isCollidable = true;
    bool isAlive = true;
    BoundingCube boundingCube;
    BoundingBox boundingBox;

    bool debugIsColliding = false;
protected:
    // Mesh
    
    // Location etc information

    // Collision
    glm::vec2 get2DPosition();
    BoundingBox getLocatedBoundingBox();

    // @NOTE: This is probably a bad idea
	void retrieveBoundingCube(models::Model model);
    
    // @TODO: Add Collision check that only checks bounding boxes if collidable
private:
    glm::mat4 getTransformationMatrix();
};

#endif // ENTITY_H
