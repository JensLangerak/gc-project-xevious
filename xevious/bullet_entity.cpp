#include "entity.h"
#include "bullet_entity.h"
#include <iostream>

BulletEntity::~BulletEntity()
{
}

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

void BulletEntity::update(double tick, Gamestate* state )
{
	// @NOTE: Perhaps move velocity into global, or into constructor (if multiple enemies of different speeds)
	float velocity = 1.5;
	float distance = velocity * tick;
	position += direction * distance;
}

void BulletEntity::onCollision(Entity* entity )
{
	isAlive = false;
	isCollidable = false;
	canBeRemoved = true;
}