#include "entity.h"
#include "models.h"
#include <math.h>
#include "utils.h"

Entity::Entity()
{
    //ctor
}

Entity::~Entity()
{
    //dtor
}

glm::mat4 Entity::getTransformationMatrix()
{
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
    
    return model;
}

void Entity::draw(long thick, glm::mat4 projView)
{
    glm::mat4 mvp = projView * getTransformationMatrix();
    
    glUniformMatrix4fv(glGetUniformLocation(globals::mainProgram, "mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(glGetUniformLocation(globals::mainProgram, "color"), 1, glm::value_ptr(color));


    models::drawModel(model);

}