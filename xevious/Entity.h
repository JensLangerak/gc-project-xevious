#ifndef ENTITY_H
#define ENTITY_H


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
    
    //collision

    private:
};

#endif // ENTITY_H
