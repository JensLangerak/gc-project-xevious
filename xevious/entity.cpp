#include "entity.h"
#include "models.h"
#include <math.h>
#include "utils.h"
#include "bounding_box.h"

// @TODO: Testing constructor. should probably be removed or changed later
Entity::Entity() : boundingBox(0, 0, 0, 0)
{
    // @TODO: Add simple block/ship/enemy model for testing in models.cpp
    // Default mesh for now: dragon
    
}

// @Testing constructor, should probably be removed or changed later
// Entity::Entity(Model::ModelType type) : boundingBox(0, 0, 0, 0)
// {
//     model = type;
//     // Fetch bounding box

// }

Entity::~Entity()
{
    //dtor
}

glm::mat4 Entity::getTransformationMatrix()
{
    // @NOTE(BUG): Assuming that a matrix is 0 by default
    glm::mat4 scaleMatrix = glm::mat4();
    scaleMatrix[0][0] = scale;
    scaleMatrix[1][1] = scale;
    scaleMatrix[2][2] = scale;
    scaleMatrix[3][3] = 1.0;

    double xRot = orientation[0];
    double yRot = orientation[1];
    double zRot = orientation[2];
    
    glm::mat4 model = glm::mat4();
    model[0][0] = cos(yRot) * cos(zRot);
    model[0][1] = cos(yRot) * sin(zRot);
    model[0][2] = -sin(yRot);
    model[1][0] = sin(xRot) * sin(yRot) * cos(zRot) - cos(xRot) * sin(zRot);
    model[1][1] = sin(xRot) * sin(yRot) * sin(zRot) + cos(xRot) * cos(zRot);
    model[1][2] = sin(xRot) * cos(yRot);
    model[2][0] = cos(xRot) * sin(yRot) * cos(zRot) + sin(xRot) * sin(zRot);
    model[2][1] = cos(xRot) * sin(yRot) * sin(zRot) - sin(xRot) * sin(zRot);
    model[2][2] = cos(xRot) * cos(yRot);
        
    model[3] = glm::vec4(position, 1.0);
    
    return model * scaleMatrix;
}

void Entity::draw(long thick, glm::mat4 projView)
{
    glm::mat4 mvp = projView * getTransformationMatrix();
    glm::mat4 modelMatrix = getTransformationMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));
    
    glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));

    models::activateTexture(texture);
    models::drawModel(model);

}


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
