#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Entity
{
    public:
        Entity();
        virtual ~Entity();
        
        void draw(long thick);
        void update(long thick);

	protected:
    //mesh
    
    //location etc information
    glm::vec3 position;
    glm::vec3 orientation;


    //collision

    private:
};

#endif // ENTITY_H
