#include "bullet_entity.h"


BulletEntity::BulletEntity(glm::vec3 pos, glm::vec3 dir)
{
	position = pos;
	direction = dir;

	model = models::ModelType::StarEnemy;
	scale = 0.02;
	color = glm::vec3(1., 0., 1.);
	boundingCube = models::makeBoundingCube(models::starEnemy.vertices);
}

void BulletEntity::update(double tick, Gamestate* state)
{
	// @NOTE: Move velocity into global, or into constructor
	float velocity = 0.001 * tick;
	position += direction * velocity;
}

void BulletEntity::onCollision(Entity* entity)
{
	isAlive = false;
	isCollidable = false;
	canBeRemoved = true;
}