#ifndef BULLET_ENTITY_H
#define BULLET_ENTITY_H

#include "entity.h"
#include "utils.h"

class BulletEntity : public Entity
{
public:
	BulletEntity(glm::vec3 pos, glm::vec3 dir);
	~BulletEntity();
	
	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);

	glm::vec3 direction;
	EntityType type = EntityType::Bullet;
};

#endif