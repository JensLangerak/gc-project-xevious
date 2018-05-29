#include "entity.h"
#include "models.h"
#include <math.h>
#include "utils.h"
#include "bounding_box.h"

// @TODO: Testing constructor. should probably be removed or changed later
Entity::Entity() : boundingBox(0, 0, 0, 0)
{
}

Entity::Entity(glm::vec3 col) : boundingBox(0, 0, 0, 0)
{
    color = col;
}

void Entity::retrieveBoundingCube(models::Model model)
{
    // @NOTE: This method is necessary since we can't construct a bounding cube before model has been loaded
    // @TODO: If we're constructing many enemies, perhaps this value should be cached in the models namespace
    boundingCube = models::makeBoundingCube(model.vertices);
}

Entity::~Entity()
{
    //dtor
}

glm::mat4 Entity::getTransformationMatrix()
{
    glm::mat4 scaleMatrix = glm::mat4();
    scaleMatrix[0][0] = scale;
    scaleMatrix[1][1] = scale;
    scaleMatrix[2][2] = scale;
    scaleMatrix[3][3] = 1.0;

    double xRot = orientation[0];
    double yRot = orientation[1];
    double zRot = orientation[2];

    glm::mat4 model = getRotationMatrix(xRot, yRot, zRot);
    model[3] = glm::vec4(position, 1.0);
    
    return model * scaleMatrix;
}

void Entity::update(double tick , Gamestate* state )
{
    // @TODO: Figure out if anything needs to be done here
}

void Entity::onCollision(Entity* entity )
{    
}

bool Entity::checkCollision(Entity* entity)
{
    return getProjectedBoundingBox().checkIntersection(entity->getProjectedBoundingBox()); 
}

void Entity::draw(long tick , glm::mat4 projView)
{
    glm::mat4 mvp = projView * getTransformationMatrix();
    glm::mat4 modelMatrix = getTransformationMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

    models::activateTexture(texture);
    models::drawModel(model);
}

void Entity::drawBoundingCube(glm::mat4 projView, glm::vec3 drawColor)
{
    boundingCube.draw(projView * getTransformationMatrix(), drawColor);   
}

BoundingBox Entity::getProjectedBoundingBox()
{
    return boundingCube.getProjectedBoundingBox(getTransformationMatrix());
}

// bool Entity::checkCollision(Entity other)
// {
//     // @TODO: Implement

// }


glm::vec2 Entity::get2DPosition()
{
    // @NOTE: We use x and z coordinated for x and y 2D coordinates
    return glm::vec2(position.x, position.z);
}

BoundingBox Entity::getLocatedBoundingBox()
{
    // @TODO: Rotate BoundingCube (assumed to be zero-centered)
    // @TODO: Extract max BoundingBox from BoundingCube;

    return BoundingBox(boundingBox, get2DPosition());
}
