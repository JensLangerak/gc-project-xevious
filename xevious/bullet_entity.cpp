#include "entity.h"
#include "bullet_entity.h"
#include <iostream>
#include "models.h"

#define PI 3.14
#define FLASH_DURATION 0.3

BulletEntity::~BulletEntity()
{
	
}

BulletEntity::BulletEntity(glm::vec3 pos, glm::vec3 dir)
{

	// Rendering
	color = glm::vec3(1., 1., 1.);
	texture = models::Textures::Beam1;
	scale = 0.02;
	hasCollided = false;
	flashRemaining = 0;

	// Gameplay
	position = pos;
	direction = dir;
	model = models::ModelType::Bullet;
	boundingCube = models::makeBoundingCube(models::starEnemy.vertices);
	type = EntityType::Bullet;

}

void BulletEntity::update(double tick, Gamestate* state )
{
	// @NOTE: Perhaps move velocity into global, or into constructor (if multiple enemies of different speeds)
	float velocity = 1.5;
	float distance = velocity * tick;
	position += direction * distance;

	if (hasCollided)
	{
		flashRemaining += tick;
		if (flashRemaining >= FLASH_DURATION)
		{
			std::cout << "removing collided bullet!\n"; 
			canBeRemoved = true;

			// Reset the bullet Lighting
			glm::vec3 resetLight = glm::vec3(-6., -6., -6.);  
			glUniform3fv(glGetUniformLocation(globals::mainProgram, "lightBulletPos"), 1, glm::value_ptr(resetLight));
		}
	}
}

void BulletEntity::draw(long tick, glm::mat4 projView)
{
	// @TODO: Make light orange (probably in shader)
	if (hasCollided && !canBeRemoved)
	{
		// @TODO: Activate light source above current location
		// don't bother drawing model instead, since collision has happened
		// Set flash radius
		float flashRadius = sin(flashRemaining * (PI / FLASH_DURATION)) * 0.1;
		glUniform1f(glGetUniformLocation(globals::mainProgram, "flashRadius"), flashRadius);

		// Set flash position
		glm::vec3 lightPosA = *lightPos; // + lightPosOffset;
		lightPosA.y = 3.0;
		glUniform3fv(glGetUniformLocation(globals::mainProgram, "lightBulletPos"), 1, glm::value_ptr(lightPosA));
		
	}

	Entity::draw(tick, projView);
}

// @TODO: Is this actually called on collision?
void BulletEntity::onCollision(Entity* entity )
{
	// @TODO: does isAlive need to be true instead?
	isAlive = false;
	isCollidable = false;
	hasCollided = true;
	
	// @TODO(Bug): This is dangerous
	lightPos = &(entity->position);
	lightPosOffset = entity->bbCenterOffset;
}