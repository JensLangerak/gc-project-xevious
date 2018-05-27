#include "bullet_entity.h"


BulletEntity::BulletEntity(glm::vec3 pos, glm::vec3 dir)
{
	position = pos;
	direction = dir;
}

void BulletEntity::update(double tick, Gamestate* state)
{
	// @NOTE: Move velocity into global, or into constructor
	float velocity = 0.05;
	position += direction * velocity;
}

void BulletEntity::onCollision(Entity* entity)
{
	isAlive = false;
	isCollidable = false;
	canBeRemoved = true;
}