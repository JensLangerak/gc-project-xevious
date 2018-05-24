#include "entity.h"
#include "models.h"
#include <math.h>
Entity::Entity()
{
    //ctor
}

Entity::~Entity()
{
    //dtor
}

void Entity::draw(long thick, glm::mat4 projView)
{
    double xRot = orientation[0]; //3 tand
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
    glm::mat4 mvp = projView * model;
    
    glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));


    // Bind vertex data
    glBindVertexArray(models::vao);


    // Execute draw command
    glDrawArrays(GL_TRIANGLES, 0, models::dragonVertices.size());
    
}