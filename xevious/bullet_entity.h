#ifndef BULLET_ENTITY_H
#define BULLET_ENTITY_H

#include "entity.h"
#include "utils.h"

class BulletEntity : public Entity
{
	BulletEntity(glm::vec3 pos, glm::vec3 dir);

	void update(double tick, Gamestate* state);
	void onCollision(Entity* entity);

	glm::vec3 direction;
	EntityType type = EntityType::Bullet;
};

#ifndef