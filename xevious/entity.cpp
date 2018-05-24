#include "entity.h"
#include "models.h"
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
    
    		glm::mat4 model = glm::mat4();
            model[0][0] = 1;
            model[1][1] = 1;
            model[2][2] = 1;
            model[3] = glm::vec4(position, 1.0);
		glm::mat4 mvp = projView * model;
        
        position[0] += 0.001;

		glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(mvp));


		// Bind vertex data
		glBindVertexArray(models::vao);


		// Execute draw command
		glDrawArrays(GL_TRIANGLES, 0, models::dragonVertices.size());
    
}