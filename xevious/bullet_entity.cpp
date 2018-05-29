#include "entity.h"
#include "bullet_entity.h"
#include <iostream>

BulletEntity::~BulletEntity()
{
}


// @TODO(Bug): Something weird happening with collisions not being perfect here
BulletEntity::BulletEntity(glm::vec3 pos, glm::vec3 dir)
{
	position = pos;
	direction = dir;

	model = models::ModelType::StarEnemy;
	scale = 0.02;
	color = glm::vec3(1., 0., 1.);
	boundingCube = models::makeBoundingCube(models::starEnemy.vertices);

	type = EntityType::Bullet;
}

void BulletEntity::update(double tick, Gamestate* state __attribute__((unused)))
{
	// @NOTE: Move velocity into global, or into constructor
	float velocity = 1.5 * tick;
	position += direction * velocity;

	// @TODO: Mark for deletion once out of screen
}

void BulletEntity::onCollision(Entity* entity __attribute__((unused)))
{
	isAlive = false;
	isCollidable = false;
	canBeRemoved = true;
}